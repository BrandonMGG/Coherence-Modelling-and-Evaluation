from states import CacheState
NBLOCKS = 4

class CacheBlock: 
    """
        @class CacheBlock is a unit of data in a cache. It contains the following fields:
        * id: The unique identifier of the cache block.
        * set: The set to which the cache block belongs.
        * state: The state of the cache block (e.g., invalid, exclusive, shared, modified).
        * address: The address of the data in the cache block.
        * data: The data in the cache block.
    """
    def __init__(self, id:int, set:int, state:str, address:str, data:str):
        self.id = id
        self.set = set
        self.state = state
        self.address = address
        self.data = data

    def get_id(self):
        return self.id

    def set_id(self, id):
        self.id = id

    def get_set(self):
        return self.set

    def get_state(self):
        return self.state

    def set_state(self, state):
        self.state = state

    def get_address(self):
        return self.address

    def set_address(self, address):
        self.address = address

    def get_data(self):
        return self.data

    def set_data(self, data):
        self.data = data
    
    def get_string(self):
        return ['B'+ str(self.id) , str(self.set) , self.state,  self.address, self.data] #Se obtiene los valores detro de la cache

class L1Cache:
    """
        @class L1Cache fast memory for inmediate CPU storage. It contains the following attributes:
        * blocks: contains a list of CacheBlocks
    """
    def __init__(self):
        self.blocks = []
        self.populate_cache()

    def populate_cache(self):
        set = 0
        for i in range(NBLOCKS):
            if i > 1:
                set = 1
            self.blocks.append(CacheBlock(i, set, CacheState.INVALID, "0000", "0000"))

    def get_block_by_id(self, id):
        return self.blocks[id]

    def get_blocks_in_set(self, _set):
        blocks = []
        for block in self.blocks:
            if _set == block.get_set():
                blocks.append(block)
        return blocks

    def get_block_by_address(self, address):
        for block in self.blocks:
            if address == block.get_address():
                return block
        return None

    def set_block(self, id, cache_block):
        self.blocks[id] = cache_block

    def set_state(self, address, value):
        block = self.get_block_by_address(address)
        self.blocks[block.get_id()].set_state(value)

    def set_data(self, address, value):
        block = self.get_block_by_address(address)
        self.blocks[block.get_id()].set_data(value)

    def set_address(self, address, value):
        self.blocks[address].set_address(value)

    def set(self, id, state, address, data):
        block = self.get_block_by_id(id)
        block.set_state(state)
        block.set_data(data)
        block.set_address(address)

    def get_hit_rate(self):
        hit_count = 0
        for block in self.blocks:
            if block.state == CacheState.EXCLUSIVE:
                hit_count += 1
        return hit_count / len(self.blocks)
    
    def pretty_print(self):
        for block in self.blocks:
            print(block.get_string())


# cache = L1Cache()
# cache.pretty_print()

# set_1 = [i.get_string() for i in cache.get_blocks_in_set(1)]
# print(set_1)
# # Agregar una entrada a la caché
# cache.set_block(0, CacheBlock(0, 0, CacheState.EXCLUSIVE, "0xf", "11fe"))
# block = cache.get_block_by_id(0)
# res = cache.get_block_by_address("0xf")
# print(res.get_string())
# assert block.id == 0
# assert block.state == CacheState.EXCLUSIVE
# assert block.address == "0xf"
# assert block.data == "11fe"

# # Comprobar la tasa de aciertos
# assert cache.get_hit_rate() == 1/4