#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <cstdint>
#include <cstddef>
#include <vector>
#include <cstring>

namespace BloomFilterImpl {

// FNV-1a hash constants
constexpr uint64_t FNV_PRIME_64 = 0x00000100000001b3ULL;
constexpr uint64_t FNV_OFFSET_BASIS_64 = 0xcbf29ce484222325ULL;

// Fast FNV-1a hash function for 64-bit
inline uint64_t fnv1a_hash(const void* data, size_t length) {
    uint64_t hash = FNV_OFFSET_BASIS_64;
    const unsigned char* bytes = static_cast<const unsigned char*>(data);
    
    for (size_t i = 0; i < length; ++i) {
        hash ^= bytes[i];
        hash *= FNV_PRIME_64;
    }
    return hash;
}

// Generate k hash values from two base hashes using double hashing technique
// This avoids computing k independent hash functions
inline void generate_k_hashes(const void* data, size_t length, uint64_t* hashes, int k, size_t bit_array_size) {
    uint64_t hash1 = fnv1a_hash(data, length);
    
    // Create a second hash by using the first hash as input
    uint64_t hash2 = fnv1a_hash(&hash1, sizeof(hash1));
    
    for (int i = 0; i < k; ++i) {
        // Use double hashing: h_i(x) = h1(x) + i * h2(x)
        hashes[i] = (hash1 + i * hash2) % bit_array_size;
    }
}

} // namespace BloomFilterImpl

template<size_t BIT_ARRAY_SIZE_BITS, int NUM_HASH_FUNCTIONS>
class BloomFilter {
private:
    static constexpr size_t BYTES_SIZE = (BIT_ARRAY_SIZE_BITS + 7) / 8; // Round up to nearest byte
    std::vector<uint8_t> bit_array_;
    
    // Set a bit at the given position
    void set_bit(size_t bit_index) {
        size_t byte_index = bit_index / 8;
        size_t bit_offset = bit_index % 8;
        if (byte_index < bit_array_.size()) {
            bit_array_[byte_index] |= (1 << bit_offset);
        }
    }
    
    // Check if a bit is set at the given position
    bool is_bit_set(size_t bit_index) const {
        size_t byte_index = bit_index / 8;
        size_t bit_offset = bit_index % 8;
        if (byte_index < bit_array_.size()) {
            return (bit_array_[byte_index] & (1 << bit_offset)) != 0;
        }
        return false;
    }

public:
    BloomFilter() : bit_array_(BYTES_SIZE, 0) {}
    
    // Add an element to the Bloom filter
    void add(const void* element) {
        uint64_t hashes[NUM_HASH_FUNCTIONS];
        BloomFilterImpl::generate_k_hashes(&element, sizeof(void*), hashes, NUM_HASH_FUNCTIONS, BIT_ARRAY_SIZE_BITS);
        
        for (int i = 0; i < NUM_HASH_FUNCTIONS; ++i) {
            set_bit(hashes[i]);
        }
    }
    
    // Check if an element might be in the set
    // Returns true if the element might be present (could be false positive)
    // Returns false if the element is definitely not present (no false negatives)
    bool check(const void* element) const {
        uint64_t hashes[NUM_HASH_FUNCTIONS];
        BloomFilterImpl::generate_k_hashes(&element, sizeof(void*), hashes, NUM_HASH_FUNCTIONS, BIT_ARRAY_SIZE_BITS);
        
        for (int i = 0; i < NUM_HASH_FUNCTIONS; ++i) {
            if (!is_bit_set(hashes[i])) {
                return false; // Definitely not in the set
            }
        }
        return true; // Might be in the set
    }
    
    // Clear all bits in the filter
    void clear() {
        std::fill(bit_array_.begin(), bit_array_.end(), 0);
    }
    
    // Get memory usage in bytes
    size_t memory_usage() const {
        return bit_array_.size();
    }
    
    // Get the number of bits set (for debugging/statistics)
    size_t count_set_bits() const {
        size_t count = 0;
        for (uint8_t byte : bit_array_) {
            // Count bits using Brian Kernighan's algorithm
            while (byte) {
                count++;
                byte &= (byte - 1);
            }
        }
        return count;
    }
    
    // Calculate approximate false positive probability based on current state
    double estimate_false_positive_rate(size_t num_inserted_elements) const {
        if (num_inserted_elements == 0) return 0.0;
        
        double bits_set_ratio = static_cast<double>(count_set_bits()) / BIT_ARRAY_SIZE_BITS;
        return std::pow(bits_set_ratio, NUM_HASH_FUNCTIONS);
    }
};

// Convenience typedefs for common configurations
// Parameters chosen for ~1M elements with ~0.1% false positive rate
// Bit array size: ~9.6M bits (1.2MB), Hash functions: 10
using DoubleFreeBloomFilter = BloomFilter<9600000, 10>;

// More conservative configuration: ~500K elements with ~0.01% false positive rate  
// Bit array size: ~7.2M bits (900KB), Hash functions: 13
using ConservativeDoubleFreeBloomFilter = BloomFilter<7200000, 13>;

// Smaller configuration for testing: ~100K elements with ~1% false positive rate
// Bit array size: ~480K bits (60KB), Hash functions: 7
using TestDoubleFreeBloomFilter = BloomFilter<480000, 7>;

// Tiny configuration for quick testing: ~1K elements with ~5% false positive rate
// Bit array size: ~5K bits (625 bytes), Hash functions: 3
using TinyTestBloomFilter = BloomFilter<5000, 3>;

// Adaptive Bloom Filter with scaling tiers
// Scales up through progressively larger configurations under pressure
class AdaptiveBloomFilter {
public:
    enum ScaleTier {
        TIER_1M = 0,    // 1M elements, 1.2MB, 10 hash functions  
        TIER_2M = 1,    // 2M elements, 2.4MB, 10 hash functions
        TIER_5M = 2,    // 5M elements, 6.0MB, 10 hash functions
        TIER_10M = 3,   // 10M elements, 12MB, 10 hash functions
        TIER_20M = 4    // 20M elements, 24MB, 10 hash functions (max)
    };

private:
    // Individual bloom filter types for each tier
    using Tier1MFilter = BloomFilter<9600000, 10>;   // 1M elements
    using Tier2MFilter = BloomFilter<19200000, 10>;  // 2M elements  
    using Tier5MFilter = BloomFilter<48000000, 10>;  // 5M elements
    using Tier10MFilter = BloomFilter<96000000, 10>; // 10M elements
    using Tier20MFilter = BloomFilter<192000000, 10>;// 20M elements

    ScaleTier current_tier_;
    size_t items_added_;
    size_t reset_count_;
    
    // Storage for current filter (using variant or union-like approach)
    std::unique_ptr<Tier1MFilter> tier1_filter_;
    std::unique_ptr<Tier2MFilter> tier2_filter_;
    std::unique_ptr<Tier5MFilter> tier5_filter_;
    std::unique_ptr<Tier10MFilter> tier10_filter_;
    std::unique_ptr<Tier20MFilter> tier20_filter_;
    
    // Cache of recently added items for data preservation during scaling
    std::vector<const void*> recent_items_cache_;
    static constexpr size_t MAX_CACHE_SIZE = 10000; // Keep last 10K items for transfer

    void initialize_tier(ScaleTier tier) {
        // Clean up previous tier
        tier1_filter_.reset();
        tier2_filter_.reset();
        tier5_filter_.reset();
        tier10_filter_.reset();
        tier20_filter_.reset();
        
        // Initialize new tier
        current_tier_ = tier;
        switch (tier) {
            case TIER_1M:
                tier1_filter_ = std::make_unique<Tier1MFilter>();
                break;
            case TIER_2M:
                tier2_filter_ = std::make_unique<Tier2MFilter>();
                break;
            case TIER_5M:
                tier5_filter_ = std::make_unique<Tier5MFilter>();
                break;
            case TIER_10M:
                tier10_filter_ = std::make_unique<Tier10MFilter>();
                break;
            case TIER_20M:
                tier20_filter_ = std::make_unique<Tier20MFilter>();
                break;
        }
    }

    size_t get_capacity_for_tier(ScaleTier tier) const {
        switch (tier) {
            case TIER_1M: return 800000;   // Clear at 80% of 1M
            case TIER_2M: return 1600000;  // Clear at 80% of 2M
            case TIER_5M: return 4000000;  // Clear at 80% of 5M
            case TIER_10M: return 8000000; // Clear at 80% of 10M
            case TIER_20M: return 16000000;// Clear at 80% of 20M
        }
        return 800000;
    }

public:
    AdaptiveBloomFilter() : current_tier_(TIER_1M), items_added_(0), reset_count_(0) {
        initialize_tier(TIER_1M);
    }

    void add(const void* element) {
        switch (current_tier_) {
            case TIER_1M: tier1_filter_->add(element); break;
            case TIER_2M: tier2_filter_->add(element); break;
            case TIER_5M: tier5_filter_->add(element); break;
            case TIER_10M: tier10_filter_->add(element); break;
            case TIER_20M: tier20_filter_->add(element); break;
        }
        items_added_++;
        
        // Cache recent items for data preservation during scaling
        recent_items_cache_.push_back(element);
        if (recent_items_cache_.size() > MAX_CACHE_SIZE) {
            recent_items_cache_.erase(recent_items_cache_.begin());
        }
        
        // Check if we need to scale up
        if (should_scale_up()) {
            scale_up();
        }
    }

    bool check(const void* element) const {
        switch (current_tier_) {
            case TIER_1M: return tier1_filter_->check(element);
            case TIER_2M: return tier2_filter_->check(element);
            case TIER_5M: return tier5_filter_->check(element);
            case TIER_10M: return tier10_filter_->check(element);
            case TIER_20M: return tier20_filter_->check(element);
        }
        return false;
    }

    void clear() {
        items_added_ = 0;
        recent_items_cache_.clear();
        switch (current_tier_) {
            case TIER_1M: tier1_filter_->clear(); break;
            case TIER_2M: tier2_filter_->clear(); break;
            case TIER_5M: tier5_filter_->clear(); break;
            case TIER_10M: tier10_filter_->clear(); break;
            case TIER_20M: tier20_filter_->clear(); break;
        }
    }

    size_t memory_usage() const {
        switch (current_tier_) {
            case TIER_1M: return tier1_filter_->memory_usage();
            case TIER_2M: return tier2_filter_->memory_usage();
            case TIER_5M: return tier5_filter_->memory_usage();
            case TIER_10M: return tier10_filter_->memory_usage();
            case TIER_20M: return tier20_filter_->memory_usage();
        }
        return 0;
    }

    double estimate_false_positive_rate(size_t num_inserted_elements) const {
        switch (current_tier_) {
            case TIER_1M: return tier1_filter_->estimate_false_positive_rate(num_inserted_elements);
            case TIER_2M: return tier2_filter_->estimate_false_positive_rate(num_inserted_elements);
            case TIER_5M: return tier5_filter_->estimate_false_positive_rate(num_inserted_elements);
            case TIER_10M: return tier10_filter_->estimate_false_positive_rate(num_inserted_elements);
            case TIER_20M: return tier20_filter_->estimate_false_positive_rate(num_inserted_elements);
        }
        return 0.0;
    }

    bool should_scale_up() const {
        if (current_tier_ == TIER_20M) return false; // Already at max tier
        if (items_added_ == 0) return false; // No items to evaluate
        
        // Scale up if false positive rate exceeds 1.5% (0.015)
        double current_fp_rate = estimate_false_positive_rate(items_added_);
        return current_fp_rate > 0.015;
    }

    void scale_up() {
        if (current_tier_ == TIER_20M) {
            // At max tier, just clear instead of scaling
            double current_fp_rate = estimate_false_positive_rate(items_added_);
            printf("DEBUG: Bloom filter at max tier (20M), FP rate %.4f%%, clearing instead of scaling\n", 
                   current_fp_rate * 100.0);
            clear();
            recent_items_cache_.clear();
            reset_count_++;
            return;
        }

        ScaleTier new_tier = static_cast<ScaleTier>(static_cast<int>(current_tier_) + 1);
        double current_fp_rate = estimate_false_positive_rate(items_added_);
        printf("DEBUG: Scaling bloom filter from tier %s to tier %s (FP rate: %.4f%%, reset #%lu)\n", 
               get_tier_name(), get_tier_name_for_tier(new_tier), current_fp_rate * 100.0, reset_count_);
        
        // Preserve recently added items before scaling
        std::vector<const void*> items_to_preserve = recent_items_cache_;
        
        initialize_tier(new_tier);
        items_added_ = 0;
        reset_count_++;
        
        // Re-add preserved items to new filter
        printf("DEBUG: Preserving %zu recent items during scaling\n", items_to_preserve.size());
        for (const void* item : items_to_preserve) {
            switch (current_tier_) {
                case TIER_1M: tier1_filter_->add(item); break;
                case TIER_2M: tier2_filter_->add(item); break;
                case TIER_5M: tier5_filter_->add(item); break;
                case TIER_10M: tier10_filter_->add(item); break;
                case TIER_20M: tier20_filter_->add(item); break;
            }
            items_added_++;
        }
        recent_items_cache_ = items_to_preserve; // Restore cache
    }

    // Statistics
    ScaleTier get_current_tier() const { return current_tier_; }
    size_t get_items_added() const { return items_added_; }
    size_t get_reset_count() const { return reset_count_; }
    size_t get_current_capacity() const { return get_capacity_for_tier(current_tier_); }
    
    const char* get_tier_name() const {
        return get_tier_name_for_tier(current_tier_);
    }
    
    const char* get_tier_name_for_tier(ScaleTier tier) const {
        switch (tier) {
            case TIER_1M: return "1M";
            case TIER_2M: return "2M";
            case TIER_5M: return "5M";
            case TIER_10M: return "10M";
            case TIER_20M: return "20M";
        }
        return "Unknown";
    }
};

#endif // BLOOM_FILTER_H