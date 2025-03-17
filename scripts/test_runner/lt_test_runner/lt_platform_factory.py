from .lt_platform import lt_platform
from .lt_platform_stm32 import lt_platform_stm32_f4, lt_platform_stm32_l4

class lt_platform_factory:
    LT_PLATFORM_STRING_ID_MAPPING = {
        "stm32_f439zi": lt_platform_stm32_f4,
        "stm32_l432kc": lt_platform_stm32_l4
    }
    
    @staticmethod
    def create_from_str_id(platform_string_id: str) -> lt_platform | None:
        try:
            return lt_platform_factory.LT_PLATFORM_STRING_ID_MAPPING[platform_string_id]()
        except KeyError:
            return None