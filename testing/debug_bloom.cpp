#include <iostream>
#include <vector>
#include <cstdint>

// Simple debug version to isolate the issue
namespace DebugBloom {

// FNV-1a constants
constexpr uint64_t FNV_PRIME_64 = 0x00000100000001b3ULL;
constexpr uint64_t FNV_OFFSET_BASIS_64 = 0xcbf29ce484222325ULL;

// Simple FNV-1a hash
uint64_t simple_hash(const void* data, size_t length) {
    std::cout << "Hash input: data=" << data << ", length=" << length << std::endl;
    
    uint64_t hash = FNV_OFFSET_BASIS_64;
    const unsigned char* bytes = static_cast<const unsigned char*>(data);
    
    for (size_t i = 0; i < length; ++i) {
        hash ^= bytes[i];
        hash *= FNV_PRIME_64;
        if (i < 10) { // Only log first few iterations
            std::cout << "  byte[" << i << "]=" << (int)bytes[i] << ", hash=" << hash << std::endl;
        }
    }
    
    std::cout << "Final hash: " << hash << std::endl;
    return hash;
}

class SimpleBloomFilter {
private:
    static constexpr size_t BITS = 1000;
    static constexpr size_t BYTES = (BITS + 7) / 8;
    static constexpr int HASHES = 3;
    
    std::vector<uint8_t> bits_;
    
public:
    SimpleBloomFilter() {
        std::cout << "Creating Bloom filter with " << BITS << " bits (" << BYTES << " bytes)" << std::endl;
        bits_.resize(BYTES, 0);
        std::cout << "Bloom filter created successfully" << std::endl;
    }
    
    void add(const void* item) {
        std::cout << "Adding item: " << item << std::endl;
        
        uint64_t hash1 = simple_hash(item, sizeof(void*));
        uint64_t hash2 = simple_hash(&hash1, sizeof(hash1));
        
        for (int i = 0; i < HASHES; ++i) {
            uint64_t bit_pos = (hash1 + i * hash2) % BITS;
            std::cout << "  Setting bit " << bit_pos << std::endl;
            
            size_t byte_idx = bit_pos / 8;
            size_t bit_offset = bit_pos % 8;
            
            if (byte_idx < bits_.size()) {
                bits_[byte_idx] |= (1 << bit_offset);
                std::cout << "  Bit set successfully" << std::endl;
            } else {
                std::cout << "  ERROR: byte_idx " << byte_idx << " >= " << bits_.size() << std::endl;
            }
        }
        std::cout << "Item added successfully" << std::endl;
    }
    
    bool check(const void* item) {
        std::cout << "Checking item: " << item << std::endl;
        
        uint64_t hash1 = simple_hash(item, sizeof(void*));
        uint64_t hash2 = simple_hash(&hash1, sizeof(hash1));
        
        for (int i = 0; i < HASHES; ++i) {
            uint64_t bit_pos = (hash1 + i * hash2) % BITS;
            std::cout << "  Checking bit " << bit_pos << std::endl;
            
            size_t byte_idx = bit_pos / 8;
            size_t bit_offset = bit_pos % 8;
            
            if (byte_idx >= bits_.size()) {
                std::cout << "  ERROR: byte_idx " << byte_idx << " >= " << bits_.size() << std::endl;
                return false;
            }
            
            bool bit_set = (bits_[byte_idx] & (1 << bit_offset)) != 0;
            std::cout << "  Bit " << bit_pos << " is " << (bit_set ? "set" : "clear") << std::endl;
            
            if (!bit_set) {
                std::cout << "Item definitely not present" << std::endl;
                return false;
            }
        }
        
        std::cout << "Item might be present" << std::endl;
        return true;
    }
};

} // namespace DebugBloom

int main() {
    std::cout << "=== Debug Bloom Filter Test ===" << std::endl;
    
    try {
        std::cout << "Step 1: Creating filter..." << std::endl;
        DebugBloom::SimpleBloomFilter bloom;
        
        std::cout << "Step 2: Testing basic operations..." << std::endl;
        void* ptr1 = (void*)0x1000;
        void* ptr2 = (void*)0x2000;
        
        std::cout << "Step 3: Initial checks..." << std::endl;
        bool check1_before = bloom.check(ptr1);
        std::cout << "ptr1 check before add: " << check1_before << std::endl;
        
        bool check2_before = bloom.check(ptr2);
        std::cout << "ptr2 check before add: " << check2_before << std::endl;
        
        std::cout << "Step 4: Adding ptr1..." << std::endl;
        bloom.add(ptr1);
        
        std::cout << "Step 5: Checking after add..." << std::endl;
        bool check1_after = bloom.check(ptr1);
        std::cout << "ptr1 check after add: " << check1_after << std::endl;
        
        bool check2_after = bloom.check(ptr2);
        std::cout << "ptr2 check after add: " << check2_after << std::endl;
        
        std::cout << "=== Test completed successfully ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "Unknown exception caught" << std::endl;
        return 1;
    }
    
    return 0;
}