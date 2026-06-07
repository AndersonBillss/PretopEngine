struct VertexInput {
    @location(0) position: vec3f,
    @location(1) normal: vec3f,
    @location(2) color: vec3f,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) normal: vec3f,
    @location(1) color: vec3f,
};

struct MyUniforms {
    projectionMatrix: mat4x4f,
    viewMatrix: mat4x4f,
    modelMatrix: mat4x4f,
    color: f32,
    time: f32,
};

@group(0) @binding(0) var<uniform> uMyUniforms: MyUniforms;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    var position = vec4f(in.position.x, in.position.y, in.position.z, 1.0);

    position = uMyUniforms.modelMatrix * position;
    position = uMyUniforms.viewMatrix * position;
    position = uMyUniforms.projectionMatrix * position;
    out.position = position;

    out.normal = (uMyUniforms.modelMatrix * vec4f(in.normal, 0.0)).xyz;
    out.color = vec3f(in.color.x * uMyUniforms.color, in.color.y, in.color.z);
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let normal = normalize(in.normal);
    let lightDirection1 = vec3f(0.5, -0.9, 0.1);
    let lightDirection2 = vec3f(0.2, 0.4, 0.3);
    let shading1 = max(0.0, dot(lightDirection1, normal));
    let shading2 = max(0.0, dot(lightDirection2, normal));
    let shading = shading1 + shading2;
    let color = in.color * shading;
    return vec4f(color, 1.0);
}
