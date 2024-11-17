cbuffer GlobalData : register(b0) {
  float4x4 global_view_projection_matrix;

  float3 ambient_light_color;

  float3 directional_light_direction;
  float3 directional_light_color;
  float directional_light_intensity;

  float3 point_light_position;
  float point_light_radius;
  float3 point_light_color;
  float point_light_intensity;

  float global_time;
  int global_frame;
};

cbuffer InstanceData : register(b1) {
  float4x4 instance_model_matrix;
  float4 instance_color;
};

Texture2D base_color_tex : register(t0);
SamplerState base_color_ss : register(s0);

struct VSInput {
  float3 position : POSITION;
  float3 normal : NORMAL;
  float2 uv : UV;
  float3 color : COLOR0;
};

struct VSOutput {
  float4 position : SV_Position;
  float3 normal : NORMAL;
  float2 uv : UV;
  float4 color : COLOR0;
  float3 world_position : TEXCOORD0;
};

struct PSOutput {
  float4 color : SV_Target0;
};

float3 do_point_light(float3 world_position, float3 world_normal) {
  float3 light_dir = point_light_position - world_position;
  float distance = length(light_dir);
  float attenuation = saturate(1.0 - (distance / point_light_radius));
  float NoL = saturate(dot(normalize(light_dir), world_normal));
  return point_light_color * point_light_intensity * attenuation * NoL;
}

float3 ACES(float3 x) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return (x * (a * x + b)) / (x * (c * x + d) + e);
}

VSOutput vs_main(VSInput input) {
  VSOutput output = (VSOutput)0;
  output.position = mul(mul(global_view_projection_matrix, instance_model_matrix), float4(input.position, 1.0));
  output.normal = normalize(mul((float3x3)instance_model_matrix, normalize(input.normal)));
  output.uv = input.uv;
  output.color = float4(input.color, 1.0) * instance_color;
  output.world_position = mul(instance_model_matrix, float4(input.position, 1.0));
  return output;
}

PSOutput ps_main(VSOutput input) {
  PSOutput output = (PSOutput)0;

  float4 base_color = base_color_tex.Sample(base_color_ss, input.uv * 1.0) * input.color;

  float3 light = 0.0;
  light += ambient_light_color;
  light += directional_light_color * directional_light_intensity * saturate(dot(input.normal, normalize(directional_light_direction)));
  light += do_point_light(input.world_position, input.normal);
  
  light *= base_color;

  output.color = float4(ACES(light), 1.0);

  // output.color = float4(input.world_position, 1.0);
  // output.color = float4(input.position.www * 0.01, 1.0);
  // output.color = float4(input.normal * 0.5 + 0.5, 1.0);

  return output;
}