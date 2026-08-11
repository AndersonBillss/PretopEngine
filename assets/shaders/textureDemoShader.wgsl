struct VertexInput {
    @location(0) position: vec2f,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) textureCoords: vec2f,
};

struct MyUniforms {
    projectionMatrix: mat4x4f,
    viewMatrix: mat4x4f,
    modelMatrix: mat4x4f,
    color: f32,
    time: f32,
};

@group(0) @binding(0) var<uniform> uMyUniforms: MyUniforms;
@group(0) @binding(1) var myTexture: texture_2d<f32>;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    var position = vec4f(in.position.x, in.position.y, 0.5, 1.0);

    position = uMyUniforms.modelMatrix * position;
    position = uMyUniforms.viewMatrix * position;
    position = uMyUniforms.projectionMatrix * position;
    
    out.position = position;
    out.textureCoords = in.position * 128;

    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let color = textureLoad(myTexture, vec2i(in.textureCoords.xy), 0).rgba;
    return color;
}
