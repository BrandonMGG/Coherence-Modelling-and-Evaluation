from enum import Enum

class CacheState(Enum):
    INVALID = "I"
    OWNED = "O"
    EXCLUSIVE = "E"
    SHARED = "S"
    MODIFIED = "M"