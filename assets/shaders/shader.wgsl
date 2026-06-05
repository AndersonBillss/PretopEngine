struct VertexInput {
    @location(0) position: vec3f,
    @location(1) color: vec3f,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) color: vec3f,
};

struct MyUniforms {
    projectionMatrix: mat4x4f,
    viewMatrix: mat4x4f,
    modelMatrix: mat4x4f,
    color: f32,
    time: f32,
};

@group(0) @binding(0) var<uniform> uMyUniforms: MyUniforms;

const pi = 3.14159265358979323846264338327950288419716939937510582097436692720969965012138599264324289254719095984578888;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    let ratio = 640.0 / 480.0; // The width and height of the target surface
    var position = vec4f(in.position.x, in.position.y, in.position.z, 1.0);

    position = uMyUniforms.modelMatrix * position;
    position = uMyUniforms.viewMatrix * position;
    position = uMyUniforms.projectionMatrix * position;
    position.z = (position.z + 1) / 10;
    out.position = position;

    out.color = vec3f(in.color.x * uMyUniforms.color, in.color.y, in.color.z);
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    return vec4f(in.color, 1.0);
}
