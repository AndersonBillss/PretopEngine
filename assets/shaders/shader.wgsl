struct VertexInput {
    @location(0) position: vec2f,
    @location(1) color: vec3f,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) color: vec3f,
};

struct MyUniforms {
    color: f32,
    time: f32,
};

@group(0) @binding(0) var<uniform> uMyUniforms: MyUniforms;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    let ratio = 640.0 / 480.0; // The width and height of the target surface
    let offset = vec2f(-0.6875, -0.463); // The offset that we want to apply to the position
    let offsetX = -cos(uMyUniforms.time) / 3;
    let offsetY = sin(uMyUniforms.time) / 3;
    out.position = vec4f(in.position.x + offset.x + offsetX, (in.position.y + offset.y + offsetY) * ratio, 0.0, 1.0);
    out.color = vec3f(in.color.x, in.color.y, in.color.z * uMyUniforms.color);
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    return vec4f(in.color, 1.0);
}
