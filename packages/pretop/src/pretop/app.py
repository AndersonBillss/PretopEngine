from pretop.shared.constants import (
    ASSET_INPUT_DIR,
    ASSET_OUTPUT_DIR,
    BUILD_DIR,
    ENGINE_DIR,
    ENGINE_NAME,
    GEN_DIR,
    SOURCE_DIR,
)


class Settings:
    def __init__(self):
        self.engine_name = ENGINE_NAME
        self.build_dir = BUILD_DIR
        self.engine_dir = ENGINE_DIR
        self.asset_input_dir = ASSET_INPUT_DIR
        self.asset_output_dir = ASSET_OUTPUT_DIR
        self.source_dir = SOURCE_DIR
        self.gen_dir = GEN_DIR


class App:
    def __init__(self):
        self.settings = Settings()
