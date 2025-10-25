#include "runtime_api.h"
#include <cstring>

/**
 * @file runtime_api.cpp
 * @brief Runtime API Implementation - The Publisher
 * 
 * This file contains the master manifest of all runtime functions.
 * It serves as the single source of truth that the compiler imports.
 * 
 * To add a new runtime function:
 * 1. Add forward declaration below
 * 2. Add entry to g_runtime_manifest array
 * 3. Implement the function in appropriate runtime/ file
 */

// =============================================================================
// FORWARD DECLARATIONS - All Runtime Function Implementations
// =============================================================================

// Forward declarations for data structures
struct ListHeader;

extern "C" {

    // Cairo Graphics Functions
    typedef uint32_t* bcpl_string_t;
    typedef uint32_t* bcpl_vector_t;
    typedef uint64_t CairoSurfaceHandle;
    typedef uint64_t CairoImageHandle;

    CairoSurfaceHandle CAIRO_CREATE_SURFACE(int64_t width, int64_t height);
    CairoSurfaceHandle CAIRO_LOAD_PNG(bcpl_string_t filename);
    void CAIRO_SAVE_PNG(CairoSurfaceHandle surface_handle, bcpl_string_t filename);
    void CAIRO_GET_SIZE(CairoSurfaceHandle surface_handle, int64_t* width, int64_t* height);
    void CAIRO_CLEAR(CairoSurfaceHandle surface_handle, uint32_t rgba_color);
    CairoSurfaceHandle CAIRO_CLONE_SURFACE(CairoSurfaceHandle source_handle);

    void CAIRO_SET_COLOR(CairoSurfaceHandle surface_handle, uint32_t rgba_color);
    void CAIRO_SET_LINE_WIDTH(CairoSurfaceHandle surface_handle, double width);
    void CAIRO_SET_LINE_JOIN(CairoSurfaceHandle surface_handle, int64_t join_style);
    void CAIRO_SET_LINE_CAP(CairoSurfaceHandle surface_handle, int64_t cap_style);
    void CAIRO_SET_OPACITY(CairoSurfaceHandle surface_handle, double opacity);
    void CAIRO_SET_ANTIALIAS(CairoSurfaceHandle surface_handle, int64_t antialias_mode);
    int64_t CAIRO_GET_ANTIALIAS(CairoSurfaceHandle surface_handle);

    void CAIRO_FILL_RECT(CairoSurfaceHandle surface_handle, double x, double y, double width, double height);
    void CAIRO_STROKE_RECT(CairoSurfaceHandle surface_handle, double x, double y, double width, double height);
    void CAIRO_FILL_CIRCLE(CairoSurfaceHandle surface_handle, double cx, double cy, double radius);
    void CAIRO_STROKE_CIRCLE(CairoSurfaceHandle surface_handle, double cx, double cy, double radius);
    void CAIRO_DRAW_LINE(CairoSurfaceHandle surface_handle, double x1, double y1, double x2, double y2);
    void CAIRO_DRAW_POINT(CairoSurfaceHandle surface_handle, double x, double y);

    void CAIRO_DRAW_LINES_VEC(CairoSurfaceHandle surface_handle, bcpl_vector_t point_pairs);
    void CAIRO_FILL_RECTS_VEC(CairoSurfaceHandle surface_handle, bcpl_vector_t rect_quads);
    void CAIRO_FILL_CIRCLES_VEC(CairoSurfaceHandle surface_handle, bcpl_vector_t circle_triples);
    void CAIRO_DRAW_POLYLINE(CairoSurfaceHandle surface_handle, bcpl_vector_t points, int64_t close_path);
    void CAIRO_FILL_POLYGON(CairoSurfaceHandle surface_handle, bcpl_vector_t points);

    void CAIRO_BEGIN_PATH(CairoSurfaceHandle surface_handle);
    void CAIRO_MOVE_TO(CairoSurfaceHandle surface_handle, double x, double y);
    void CAIRO_LINE_TO(CairoSurfaceHandle surface_handle, double x, double y);
    void CAIRO_CURVE_TO(CairoSurfaceHandle surface_handle, double x1, double y1, double x2, double y2, double x3, double y3);
    void CAIRO_CLOSE_PATH(CairoSurfaceHandle surface_handle);
    void CAIRO_FILL_PATH(CairoSurfaceHandle surface_handle);
    void CAIRO_STROKE_PATH(CairoSurfaceHandle surface_handle);

    void CAIRO_SET_FONT(CairoSurfaceHandle surface_handle, bcpl_string_t font_name, double font_size);
    void CAIRO_DRAW_TEXT(CairoSurfaceHandle surface_handle, double x, double y, bcpl_string_t text);
    void CAIRO_TEXT_SIZE(CairoSurfaceHandle surface_handle, bcpl_string_t text, double* width, double* height);
    void CAIRO_DRAW_TEXT_COLORED(CairoSurfaceHandle surface_handle, double x, double y, bcpl_string_t text, uint32_t rgba_color);

    CairoImageHandle CAIRO_LOAD_IMAGE(bcpl_string_t filename);
    void CAIRO_DRAW_IMAGE(CairoSurfaceHandle surface_handle, CairoImageHandle image_handle, double x, double y);
    void CAIRO_DRAW_IMAGE_SCALED(CairoSurfaceHandle surface_handle, CairoImageHandle image_handle, double x, double y, double scale_x, double scale_y);
    void CAIRO_DRAW_IMAGE_ROTATED(CairoSurfaceHandle surface_handle, CairoImageHandle image_handle, double x, double y, double angle);
    void CAIRO_IMAGE_SIZE(CairoImageHandle image_handle, int64_t* width, int64_t* height);

    void CAIRO_SAVE(CairoSurfaceHandle surface_handle);
    void CAIRO_RESTORE(CairoSurfaceHandle surface_handle);
    void CAIRO_TRANSLATE(CairoSurfaceHandle surface_handle, double tx, double ty);
    void CAIRO_SCALE(CairoSurfaceHandle surface_handle, double sx, double sy);
    void CAIRO_ROTATE(CairoSurfaceHandle surface_handle, double angle);

    void CAIRO_RELEASE_SURFACE(CairoSurfaceHandle surface_handle);
    void CAIRO_RELEASE_IMAGE(CairoImageHandle image_handle);
    int64_t CAIRO_SURFACE_COUNT(void);
    int64_t CAIRO_IMAGE_COUNT(void);

    int64_t CAIRO_IS_AVAILABLE(void);
    bcpl_string_t CAIRO_GET_VERSION(void);
    bcpl_string_t CAIRO_GET_ERROR(void);
    void CAIRO_CLEAR_ERROR(void);

    // SAMM integration functions
    CairoSurfaceHandle CAIRO_CREATE_SURFACE_RETAINED(int64_t width, int64_t height, int64_t parent_scope_offset);
    CairoSurfaceHandle CAIRO_LOAD_PNG_RETAINED(bcpl_string_t filename, int64_t parent_scope_offset);
    CairoImageHandle CAIRO_LOAD_IMAGE_RETAINED(bcpl_string_t filename, int64_t parent_scope_offset);
    int64_t CAIRO_SAMM_SCOPE_DEPTH(void);
    void CAIRO_SAMM_FORCE_CLEANUP(void);
    void CAIRO_SAMM_GET_STATS(int64_t* active_surfaces, int64_t* active_images, int64_t* memory_usage);
    void CAIRO_SAMM_SET_TRACE(int64_t enabled);
    void CAIRO_SAMM_DUMP_STATE(void);

    // SDL2 Display Integration functions
    void CAIRO_DISPLAY_SDL(CairoSurfaceHandle surface_handle, int64_t sdl_window_id);
    int64_t CAIRO_CREATE_SDL_WINDOW(bcpl_string_t title, int64_t width, int64_t height);
    void CAIRO_UPDATE_SDL_WINDOW(CairoSurfaceHandle surface_handle, int64_t sdl_window_id);
    int64_t CAIRO_TO_SDL_TEXTURE(CairoSurfaceHandle surface_handle, int64_t sdl_renderer_id);
    
    // Core I/O functions
    void WRITES(int string_ptr);
    void WRITEN(int value);
    void FWRITE(float value);
    void WRITEF(int format_ptr);
    void WRITEF1(int format_ptr, int arg1);
    void WRITEF2(int format_ptr, int arg1, int arg2);
    void WRITEF3(int format_ptr, int arg1, int arg2, int arg3);
    void WRITEF4(int format_ptr, int arg1, int arg2, int arg3, int arg4);
    void WRITEF5(int format_ptr, int arg1, int arg2, int arg3, int arg4, int arg5);
    void WRITEF6(int format_ptr, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);
    void WRITEF7(int format_ptr, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7);
    void WRITEC(int char_value);
    int RDCH();
    void NEWLINE();
    
    // String/List operations
    struct ListHeader* SPLIT(uint32_t* string_payload, uint32_t* delimiter_payload);
    uint32_t* JOIN(struct ListHeader* list_header, uint32_t* delimiter_payload);
    int STRLEN(int string_ptr);
    int STRCMP(int str1_ptr, int str2_ptr);
    void STRCOPY(int dest_ptr, int src_ptr);
    int PACKSTRING(int string_ptr);
    int UNPACKSTRING(int packed_ptr);
    int SLURP(int filename_ptr);
    int SPIT(int filename_ptr, int content_ptr);
    
    // Memory management
    int bcpl_alloc_words(int count, int size, int type);
    int bcpl_alloc_chars(int count);
    int bcpl_getvec(int size);
    int bcpl_fgetvec(int size);
    void bcpl_free(int ptr);
    void bcpl_free_list(int list_ptr);
    void bcpl_free_list_safe(int list_ptr);
    int BCPL_GET_LAST_ERROR(int buffer_ptr);
    void BCPL_CLEAR_ERRORS();
    void BCPL_CHECK_AND_DISPLAY_ERRORS();
    void BCPL_BOUNDS_ERROR(int index, int size, int context);
    void BCPL_FREE_CELLS();
    int get_g_free_list_head_address();
    
    // List operations
    int BCPL_LIST_CREATE_EMPTY();
    int BCPL_LIST_APPEND_INT(int list_ptr, int value);
    int BCPL_LIST_APPEND_FLOAT(int list_ptr, float value);
    int BCPL_LIST_APPEND_STRING(int list_ptr, int string_ptr);
    int BCPL_LIST_APPEND_LIST(int list_ptr, int other_list);
    int BCPL_LIST_GET_HEAD_AS_INT(int list_ptr);
    float BCPL_LIST_GET_HEAD_AS_FLOAT(int list_ptr);
    int BCPL_LIST_GET_TAIL(int list_ptr);
    int BCPL_LIST_GET_REST(int list_ptr);
    int BCPL_GET_ATOM_TYPE(int atom_ptr);
    int BCPL_LIST_GET_NTH(int list_ptr, int index);
    int BCPL_CONCAT_LISTS(int list1_ptr, int list2_ptr);
    int BCPL_SHALLOW_COPY_LIST(int list_ptr);
    int BCPL_DEEP_COPY_LIST(int list_ptr);
    int BCPL_DEEP_COPY_LITERAL_LIST(int list_ptr);
    int BCPL_REVERSE_LIST(int list_ptr);
    int BCPL_FIND_IN_LIST(int list_ptr, int value, int compare_func);
    int BCPL_LIST_FILTER(int list_ptr, int filter_func);
    
    // Vector type list functions
    int BCPL_LIST_APPEND_PAIR(int list_ptr, int pair_ptr);
    int BCPL_LIST_APPEND_FPAIR(int list_ptr, int fpair_ptr);
    int BCPL_LIST_APPEND_QUAD(int list_ptr, int quad_ptr);
    int BCPL_LIST_APPEND_FQUAD(int list_ptr, int fquad_ptr);
    int BCPL_LIST_APPEND_OCT(int list_ptr, int oct_ptr);
    int BCPL_LIST_APPEND_FOCT(int list_ptr, int foct_ptr);
    int BCPL_LIST_GET_HEAD_AS_PAIR(int list_ptr);
    int BCPL_LIST_GET_HEAD_AS_FPAIR(int list_ptr);
    int BCPL_LIST_GET_HEAD_AS_QUAD(int list_ptr);
    int BCPL_LIST_GET_HEAD_AS_FQUAD(int list_ptr);
    int BCPL_LIST_GET_HEAD_AS_OCT(int list_ptr);
    int BCPL_LIST_GET_HEAD_AS_FOCT(int list_ptr);
    void returnNodeToFreelist_runtime(int node_ptr);
    
    // Math functions
    int RAND(int max_val);
    float RND(int max_val);
    float FRND();
    float FSIN(float angle);
    float FCOS(float angle);
    float FTAN(float angle);
    float FABS(float value);
    float FLOG(float value);
    float FEXP(float value);
    int FIX(float value);
    
    // File I/O
    int FILE_OPEN_READ(int filename_ptr);
    int FILE_OPEN_WRITE(int filename_ptr);
    int FILE_OPEN_APPEND(int filename_ptr);
    int FILE_CLOSE(int handle);
    int FILE_WRITES(int handle, int string_ptr);
    int FILE_READS(int handle);
    int FILE_READ(int handle, int buffer_ptr, int size);
    int FILE_WRITE(int handle, int buffer_ptr, int size);
    int FILE_SEEK(int handle, int offset, int origin);
    int FILE_TELL(int handle);
    int FILE_EOF(int handle);
    
    // System functions
    void finish();
    
    // Timing and Performance Metrics functions
    void TIMER_START(int name_str);
    void TIMER_END(int name_str);
    void TIMER_DISPLAY();
    void TIMER_CLEAR();
    int TIMER_GET_TOTAL_NS(int name_str);
    int TIMER_GET_CALL_COUNT(int name_str);
    
    // Heap Manager functions
    void HeapManager_enterScope();
    void HeapManager_exitScope();
    int HeapManager_isSAMMEnabled();
    void HeapManager_setSAMMEnabled(int enabled);
    void HeapManager_waitForSAMM();
    int HeapManager_OBJECT_HEAP_ALLOC(int size);
    void HeapManager_OBJECT_HEAP_FREE(int ptr);
    int HeapManager_RUNTIME_METHOD_LOOKUP(int class_id, int method_id);
    int HeapManager_PIC_RUNTIME_HELPER();
    
    // Debug functions for vtable store verification
    void DEBUG_PRINT_VTABLE_STORE(int object_ptr, int original_vtable, int readback_vtable);
    void DEBUG_PRINT_FINAL_OBJECT(int object_ptr, int vtable_ptr);

#ifdef SDL2_RUNTIME_ENABLED
    // SDL2 functions
    int64_t SDL2_INIT(void);
    void SDL2_INIT_SUBSYSTEMS(int64_t flags);
    void SDL2_QUIT(void);
    int64_t SDL2_CREATE_WINDOW(uint32_t* title);
    int64_t SDL2_CREATE_WINDOW_EX(uint32_t* title, int64_t x, int64_t y, int64_t width, int64_t height, int64_t flags);
    void SDL2_DESTROY_WINDOW(int64_t window_id);
    void SDL2_SET_WINDOW_TITLE(int64_t window_id, uint32_t* title);
    void SDL2_SET_WINDOW_SIZE(int64_t window_id, int64_t width, int64_t height);
    int64_t SDL2_CREATE_RENDERER(int64_t window_id);
    int64_t SDL2_CREATE_RENDERER_EX(int64_t window_id, int64_t flags);
    void SDL2_DESTROY_RENDERER(int64_t renderer_id);
    void SDL2_SET_DRAW_COLOR(int64_t renderer_id, int64_t r, int64_t g, int64_t b, int64_t a);
    void SDL2_CLEAR(int64_t renderer_id);
    void SDL2_PRESENT(int64_t renderer_id);
    void SDL2_DRAW_POINT(int64_t renderer_id, int64_t x, int64_t y);
    void SDL2_DRAW_LINE(int64_t renderer_id, int64_t x1, int64_t y1, int64_t x2, int64_t y2);
    void SDL2_DRAW_RECT(int64_t renderer_id, int64_t x, int64_t y, int64_t w, int64_t h);
    void SDL2_FILL_RECT(int64_t renderer_id, int64_t x, int64_t y, int64_t w, int64_t h);
    int64_t SDL2_POLL_EVENT(void);
    int64_t SDL2_GET_EVENT_KEY(void);
    void SDL2_GET_EVENT_MOUSE(int64_t* x, int64_t* y);
    int64_t SDL2_GET_EVENT_BUTTON(void);
    void SDL2_DELAY(int64_t milliseconds);
    int64_t SDL2_GET_TICKS(void);
    uint32_t* SDL2_GET_ERROR(void);
    void SDL2_CLEAR_ERROR(void);
    int64_t SDL2_GET_VERSION(void);
    int64_t SDL2_GET_VIDEO_DRIVERS(void);
    uint32_t* SDL2_GET_CURRENT_VIDEO_DRIVER(void);
    int64_t SDL2_GET_DISPLAY_MODES(void);
    int64_t SDL2_TEST_BASIC(void);
#endif
}

// =============================================================================
// RUNTIME FUNCTION MANIFEST - SINGLE SOURCE OF TRUTH
// =============================================================================

// =============================================================================
// PARAMETER TYPE DEFINITIONS
// =============================================================================

// Parameter types for Cairo functions
static const RuntimeParameterType cairo_create_surface_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::INTEGER
};

static const RuntimeParameterType cairo_fill_rect_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE, 
    RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE
};

static const RuntimeParameterType cairo_stroke_rect_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE, 
    RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE
};

static const RuntimeParameterType cairo_fill_circle_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE, 
    RuntimeParameterType::DOUBLE
};

static const RuntimeParameterType cairo_stroke_circle_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE, 
    RuntimeParameterType::DOUBLE
};

static const RuntimeParameterType cairo_draw_line_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE, 
    RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE
};

static const RuntimeParameterType cairo_draw_point_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE
};

static const RuntimeParameterType cairo_set_opacity_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE
};

static const RuntimeParameterType cairo_set_line_width_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE
};

static const RuntimeParameterType cairo_draw_text_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE, 
    RuntimeParameterType::STRING
};

static const RuntimeParameterType cairo_set_font_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::STRING, RuntimeParameterType::DOUBLE
};

static const RuntimeParameterType cairo_surface_handle_only_params[] = {
    RuntimeParameterType::INTEGER
};

static const RuntimeParameterType cairo_surface_color_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::INTEGER
};

static const RuntimeParameterType cairo_surface_int_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::INTEGER
};

static const RuntimeParameterType cairo_surface_string_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::STRING
};

static const RuntimeParameterType cairo_surface_vector_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::VECTOR
};

static const RuntimeParameterType cairo_draw_image_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE, 
    RuntimeParameterType::DOUBLE
};

// Additional Cairo parameter types
static const RuntimeParameterType cairo_draw_image_scaled_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE, 
    RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE
};

static const RuntimeParameterType cairo_draw_image_rotated_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE, 
    RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE
};

static const RuntimeParameterType cairo_curve_to_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE,
    RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE
};

static const RuntimeParameterType cairo_draw_text_colored_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE, 
    RuntimeParameterType::STRING, RuntimeParameterType::INTEGER
};

static const RuntimeParameterType cairo_translate_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE
};

static const RuntimeParameterType cairo_scale_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE, RuntimeParameterType::DOUBLE
};

static const RuntimeParameterType cairo_rotate_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::DOUBLE
};

static const RuntimeParameterType cairo_antialias_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::INTEGER
};

static const RuntimeParameterType cairo_polyline_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::VECTOR, RuntimeParameterType::INTEGER
};

static const RuntimeParameterType cairo_sdl_window_params[] = {
    RuntimeParameterType::STRING, RuntimeParameterType::INTEGER, RuntimeParameterType::INTEGER
};

static const RuntimeParameterType cairo_sdl_display_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::INTEGER
};

static const RuntimeParameterType cairo_string_only_params[] = {
    RuntimeParameterType::STRING
};

static const RuntimeParameterType cairo_image_handle_only_params[] = {
    RuntimeParameterType::INTEGER
};

static const RuntimeParameterType cairo_retained_surface_params[] = {
    RuntimeParameterType::INTEGER, RuntimeParameterType::INTEGER, RuntimeParameterType::INTEGER
};

static const RuntimeParameterType cairo_retained_png_params[] = {
    RuntimeParameterType::STRING, RuntimeParameterType::INTEGER
};

static const RuntimeParameterType cairo_retained_image_params[] = {
    RuntimeParameterType::STRING, RuntimeParameterType::INTEGER
};

/**
 * Master manifest of all runtime functions published by the runtime library.
 * This is the authoritative list that the compiler imports.
 * 
 * IMPORTANT: Every runtime function callable from BCPL must have an entry here.
 * The compiler will ONLY know about functions listed in this manifest.
 */
static const RuntimeFunctionDescriptor g_runtime_manifest[] = {
    
    // -------------------------------------------------------------------------
    // CORE I/O FUNCTIONS
    // -------------------------------------------------------------------------
    {
        "WRITES", "_WRITES", reinterpret_cast<RuntimeFunctionPtr>(WRITES), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write a string to standard output", "I/O", nullptr
    },
    {
        "WRITEN", "_WRITEN", reinterpret_cast<RuntimeFunctionPtr>(WRITEN), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write an integer to standard output", "I/O", nullptr
    },
    {
        "FWRITE", "_FWRITE", reinterpret_cast<RuntimeFunctionPtr>(FWRITE), 1,
        RuntimeFunctionType::FLOAT_ROUTINE, RuntimeReturnType::VOID,
        "Write a float to standard output", "I/O", nullptr
    },
    {
        "WRITEF", "_WRITEF", reinterpret_cast<RuntimeFunctionPtr>(WRITEF), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write formatted output (base case)", "I/O", nullptr
    },
    {
        "WRITEF1", "_WRITEF1", reinterpret_cast<RuntimeFunctionPtr>(WRITEF1), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write formatted output with 1 argument", "I/O", nullptr
    },
    {
        "WRITEF2", "_WRITEF2", reinterpret_cast<RuntimeFunctionPtr>(WRITEF2), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write formatted output with 2 arguments", "I/O", nullptr
    },
    {
        "WRITEF3", "_WRITEF3", reinterpret_cast<RuntimeFunctionPtr>(WRITEF3), 4,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write formatted output with 3 arguments", "I/O", nullptr
    },
    {
        "WRITEF4", "_WRITEF4", reinterpret_cast<RuntimeFunctionPtr>(WRITEF4), 5,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write formatted output with 4 arguments", "I/O", nullptr
    },
    {
        "WRITEF5", "_WRITEF5", reinterpret_cast<RuntimeFunctionPtr>(WRITEF5), 6,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write formatted output with 5 arguments", "I/O", nullptr
    },
    {
        "WRITEF6", "_WRITEF6", reinterpret_cast<RuntimeFunctionPtr>(WRITEF6), 7,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write formatted output with 6 arguments", "I/O", nullptr
    },
    {
        "WRITEF7", "_WRITEF7", reinterpret_cast<RuntimeFunctionPtr>(WRITEF7), 8,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write formatted output with 7 arguments", "I/O", nullptr
    },
    {
        "WRITEC", "_WRITEC", reinterpret_cast<RuntimeFunctionPtr>(WRITEC), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write a character to standard output", "I/O", nullptr
    },
    {
        "RDCH", "_RDCH", reinterpret_cast<RuntimeFunctionPtr>(RDCH), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Read a character from standard input", "I/O", nullptr
    },
    {
        "NEWLINE", "_NEWLINE", reinterpret_cast<RuntimeFunctionPtr>(NEWLINE), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Output a newline character", "I/O", nullptr
    },

    // -------------------------------------------------------------------------
    // STRING AND LIST OPERATIONS
    // -------------------------------------------------------------------------
    {
        "SPLIT", "_SPLIT", reinterpret_cast<RuntimeFunctionPtr>(SPLIT), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Split a string into a list using delimiter", "String", nullptr
    },
    {
        "JOIN", "_JOIN", reinterpret_cast<RuntimeFunctionPtr>(JOIN), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING,
        "Join a list of strings using delimiter", "String", nullptr
    },
    {
        "STRLEN", "_STRLEN", reinterpret_cast<RuntimeFunctionPtr>(STRLEN), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get length of a string", "String", nullptr
    },
    {
        "STRCMP", "_STRCMP", reinterpret_cast<RuntimeFunctionPtr>(STRCMP), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Compare two strings", "String", nullptr
    },
    {
        "STRCOPY", "_STRCOPY", reinterpret_cast<RuntimeFunctionPtr>(STRCOPY), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Copy one string to another", "String", nullptr
    },
    {
        "PACKSTRING", "_PACKSTRING", reinterpret_cast<RuntimeFunctionPtr>(PACKSTRING), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING,
        "Pack a string into memory", "String", nullptr
    },
    {
        "UNPACKSTRING", "_UNPACKSTRING", reinterpret_cast<RuntimeFunctionPtr>(UNPACKSTRING), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING,
        "Unpack a string from memory", "String", nullptr
    },
    {
        "SLURP", "_SLURP", reinterpret_cast<RuntimeFunctionPtr>(SLURP), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING,
        "Read entire file into string", "String", nullptr
    },
    {
        "SPIT", "_SPIT", reinterpret_cast<RuntimeFunctionPtr>(SPIT), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write string to file", "String", nullptr
    },

    // -------------------------------------------------------------------------
    // MEMORY MANAGEMENT
    // -------------------------------------------------------------------------
    {
        "GETVEC", "_GETVEC", reinterpret_cast<RuntimeFunctionPtr>(bcpl_getvec), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INT_VECTOR,
        "Allocate integer vector", "Memory", nullptr
    },
    {
        "FGETVEC", "_FGETVEC", reinterpret_cast<RuntimeFunctionPtr>(bcpl_fgetvec), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::FLOAT_VECTOR,
        "Allocate float vector", "Memory", nullptr
    },
    {
        "FREEVEC", "_FREEVEC", reinterpret_cast<RuntimeFunctionPtr>(bcpl_free), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Free allocated vector", "Memory", nullptr
    },
    {
        "BCPL_ALLOC_WORDS", "_BCPL_ALLOC_WORDS", reinterpret_cast<RuntimeFunctionPtr>(bcpl_alloc_words), 3,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Allocate words with type info", "Memory", nullptr
    },
    {
        "BCPL_ALLOC_CHARS", "_BCPL_ALLOC_CHARS", reinterpret_cast<RuntimeFunctionPtr>(bcpl_alloc_chars), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Allocate character buffer", "Memory", nullptr
    },
    {
        "MALLOC", "_MALLOC", reinterpret_cast<RuntimeFunctionPtr>(bcpl_alloc_words), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Allocate memory (alias for BCPL_ALLOC_WORDS)", "Memory", nullptr
    },
    {
        "BCPL_FREE_LIST", "_BCPL_FREE_LIST", reinterpret_cast<RuntimeFunctionPtr>(bcpl_free_list), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Free a list structure", "Memory", nullptr
    },
    {
        "BCPL_FREE_LIST_SAFE", "_BCPL_FREE_LIST_SAFE", reinterpret_cast<RuntimeFunctionPtr>(bcpl_free_list_safe), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Safely free a list structure", "Memory", nullptr
    },
    {
        "BCPL_GET_LAST_ERROR", "_BCPL_GET_LAST_ERROR", reinterpret_cast<RuntimeFunctionPtr>(BCPL_GET_LAST_ERROR), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get last error message", "Memory", nullptr
    },
    {
        "BCPL_CLEAR_ERRORS", "_BCPL_CLEAR_ERRORS", reinterpret_cast<RuntimeFunctionPtr>(BCPL_CLEAR_ERRORS), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Clear error state", "Memory", nullptr
    },
    {
        "BCPL_CHECK_AND_DISPLAY_ERRORS", "_BCPL_CHECK_AND_DISPLAY_ERRORS", reinterpret_cast<RuntimeFunctionPtr>(BCPL_CHECK_AND_DISPLAY_ERRORS), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Check and display any errors", "Memory", nullptr
    },
    {
        "BCPL_BOUNDS_ERROR", "_BCPL_BOUNDS_ERROR", reinterpret_cast<RuntimeFunctionPtr>(BCPL_BOUNDS_ERROR), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Report bounds checking error", "Memory", nullptr
    },
    {
        "BCPL_FREE_CELLS", "_BCPL_FREE_CELLS", reinterpret_cast<RuntimeFunctionPtr>(BCPL_FREE_CELLS), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Free memory cells", "Memory", nullptr
    },
    {
        "GET_FREE_LIST_HEAD_ADDR", "_GET_FREE_LIST_HEAD_ADDR", reinterpret_cast<RuntimeFunctionPtr>(get_g_free_list_head_address), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get address of free list head", "Memory", nullptr
    },

    // -------------------------------------------------------------------------
    // LIST OPERATIONS
    // -------------------------------------------------------------------------
    {
        "LIST_CREATE", "_LIST_CREATE", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_CREATE_EMPTY), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Create empty list", "List", nullptr
    },
    {
        "LIST_APPEND_INT", "_LIST_APPEND_INT", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_INT), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append integer to list", "List", nullptr
    },
    {
        "LIST_APPEND_FLOAT", "_LIST_APPEND_FLOAT", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_FLOAT), 2,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::STRING_LIST,
        "Append float to list", "List", nullptr
    },
    {
        "LIST_APPEND_STRING", "_LIST_APPEND_STRING", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_STRING), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append string to list", "List", nullptr
    },
    {
        "LIST_HEAD_INT", "_LIST_HEAD_INT", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_GET_HEAD_AS_INT), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get first element of list as integer", "List", nullptr
    },
    {
        "LIST_HEAD_FLOAT", "_LIST_HEAD_FLOAT", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_GET_HEAD_AS_FLOAT), 1,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Get first element of list as float", "List", nullptr
    },
    {
        "LIST_TAIL", "_LIST_TAIL", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_GET_TAIL), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Get tail (rest) of list", "List", nullptr
    },
    {
        "LIST_FREE", "_LIST_FREE", reinterpret_cast<RuntimeFunctionPtr>(bcpl_free_list), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Free a list and all its elements", "List", nullptr
    },
    {
        "BCPL_LIST_APPEND_LIST", "_BCPL_LIST_APPEND_LIST", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_LIST), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append list to another list", "List", nullptr
    },
    {
        "BCPL_LIST_GET_REST", "_BCPL_LIST_GET_REST", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_GET_REST), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Get rest of list (alias for tail)", "List", nullptr
    },
    {
        "BCPL_GET_ATOM_TYPE", "_BCPL_GET_ATOM_TYPE", reinterpret_cast<RuntimeFunctionPtr>(BCPL_GET_ATOM_TYPE), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get type of atomic value", "List", nullptr
    },
    {
        "BCPL_LIST_GET_NTH", "_BCPL_LIST_GET_NTH", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_GET_NTH), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get nth element of list", "List", nullptr
    },
    {
        "BCPL_CONCAT_LISTS", "_BCPL_CONCAT_LISTS", reinterpret_cast<RuntimeFunctionPtr>(BCPL_CONCAT_LISTS), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Concatenate two lists", "List", nullptr
    },
    {
        "CONCAT", "_CONCAT", reinterpret_cast<RuntimeFunctionPtr>(BCPL_CONCAT_LISTS), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Concatenate two lists (alias)", "List", nullptr
    },
    {
        "COPYLIST", "_COPYLIST", reinterpret_cast<RuntimeFunctionPtr>(BCPL_SHALLOW_COPY_LIST), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Create shallow copy of list", "List", nullptr
    },
    {
        "DEEPCOPYLIST", "_DEEPCOPYLIST", reinterpret_cast<RuntimeFunctionPtr>(BCPL_DEEP_COPY_LIST), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Create deep copy of list", "List", nullptr
    },
    {
        "DEEPCOPYLITERALLIST", "_DEEPCOPYLITERALLIST", reinterpret_cast<RuntimeFunctionPtr>(BCPL_DEEP_COPY_LITERAL_LIST), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Create deep copy of literal list", "List", nullptr
    },
    {
        "REVERSE", "_REVERSE", reinterpret_cast<RuntimeFunctionPtr>(BCPL_REVERSE_LIST), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Reverse a list", "List", nullptr
    },
    {
        "FIND", "_FIND", reinterpret_cast<RuntimeFunctionPtr>(BCPL_FIND_IN_LIST), 3,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Find element in list", "List", nullptr
    },
    {
        "FILTER", "_FILTER", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_FILTER), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Filter list elements", "List", nullptr
    },
    {
        "APND", "_APND", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_INT), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append integer to list (alias)", "List", nullptr
    },
    {
        "FPND", "_FPND", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_FLOAT), 2,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::STRING_LIST,
        "Append float to list (alias)", "List", nullptr
    },
    {
        "SPND", "_SPND", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_STRING), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append string to list (alias)", "List", nullptr
    },
    {
        "LPND", "_LPND", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_LIST), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append list to list (alias)", "List", nullptr
    },
    {
        "BCPL_LIST_CREATE_EMPTY", "_BCPL_LIST_CREATE_EMPTY", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_CREATE_EMPTY), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Create empty list", "List", nullptr
    },
    
    // Vector type list functions
    {
        "BCPL_LIST_APPEND_PAIR", "_BCPL_LIST_APPEND_PAIR", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_PAIR), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append PAIR to list", "List", nullptr
    },
    {
        "BCPL_LIST_APPEND_FPAIR", "_BCPL_LIST_APPEND_FPAIR", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_FPAIR), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append FPAIR to list", "List", nullptr
    },
    {
        "BCPL_LIST_APPEND_QUAD", "_BCPL_LIST_APPEND_QUAD", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_QUAD), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append QUAD to list", "List", nullptr
    },
    {
        "BCPL_LIST_APPEND_FQUAD", "_BCPL_LIST_APPEND_FQUAD", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_FQUAD), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append FQUAD to list", "List", nullptr
    },
    {
        "BCPL_LIST_APPEND_OCT", "_BCPL_LIST_APPEND_OCT", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_OCT), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append OCT to list", "List", nullptr
    },
    {
        "BCPL_LIST_APPEND_FOCT", "_BCPL_LIST_APPEND_FOCT", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_FOCT), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append FOCT to list", "List", nullptr
    },
    {
        "BCPL_LIST_GET_HEAD_AS_PAIR", "_BCPL_LIST_GET_HEAD_AS_PAIR", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_GET_HEAD_AS_PAIR), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get first element of list as PAIR", "List", nullptr
    },
    {
        "BCPL_LIST_GET_HEAD_AS_FPAIR", "_BCPL_LIST_GET_HEAD_AS_FPAIR", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_GET_HEAD_AS_FPAIR), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get first element of list as FPAIR", "List", nullptr
    },
    {
        "BCPL_LIST_GET_HEAD_AS_QUAD", "_BCPL_LIST_GET_HEAD_AS_QUAD", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_GET_HEAD_AS_QUAD), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get first element of list as QUAD", "List", nullptr
    },
    {
        "BCPL_LIST_GET_HEAD_AS_FQUAD", "_BCPL_LIST_GET_HEAD_AS_FQUAD", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_GET_HEAD_AS_FQUAD), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get first element of list as FQUAD", "List", nullptr
    },
    {
        "BCPL_LIST_GET_HEAD_AS_OCT", "_BCPL_LIST_GET_HEAD_AS_OCT", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_GET_HEAD_AS_OCT), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get first element of list as OCT", "List", nullptr
    },
    {
        "BCPL_LIST_GET_HEAD_AS_FOCT", "_BCPL_LIST_GET_HEAD_AS_FOCT", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_GET_HEAD_AS_FOCT), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get first element of list as FOCT", "List", nullptr
    },
    {
        "returnNodeToFreelist", "_returnNodeToFreelist", reinterpret_cast<RuntimeFunctionPtr>(returnNodeToFreelist_runtime), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Return node to freelist", "List", nullptr
    },

    // -------------------------------------------------------------------------
    // MATHEMATICAL FUNCTIONS
    // -------------------------------------------------------------------------
    {
        "RAND", "_RAND", reinterpret_cast<RuntimeFunctionPtr>(RAND), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Generate random integer (0 to max_val-1)", "Math", nullptr
    },
    {
        "RND", "_RND", reinterpret_cast<RuntimeFunctionPtr>(RND), 1,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Generate random float (0.0 to max_val)", "Math", nullptr
    },
    {
        "FRND", "_FRND", reinterpret_cast<RuntimeFunctionPtr>(FRND), 0,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Generate random float (0.0 to 1.0)", "Math", nullptr
    },
    {
        "FSIN", "_FSIN", reinterpret_cast<RuntimeFunctionPtr>(FSIN), 1,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Sine function (radians)", "Math", nullptr
    },
    {
        "FCOS", "_FCOS", reinterpret_cast<RuntimeFunctionPtr>(FCOS), 1,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Cosine function (radians)", "Math", nullptr
    },
    {
        "FTAN", "_FTAN", reinterpret_cast<RuntimeFunctionPtr>(FTAN), 1,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Tangent function (radians)", "Math", nullptr
    },
    {
        "FABS", "_FABS", reinterpret_cast<RuntimeFunctionPtr>(FABS), 1,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Absolute value (float)", "Math", nullptr
    },
    {
        "FLOG", "_FLOG", reinterpret_cast<RuntimeFunctionPtr>(FLOG), 1,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Natural logarithm", "Math", nullptr
    },
    {
        "FEXP", "_FEXP", reinterpret_cast<RuntimeFunctionPtr>(FEXP), 1,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Exponential function", "Math", nullptr
    },
    {
        "FIX", "_FIX", reinterpret_cast<RuntimeFunctionPtr>(FIX), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Convert float to integer", "Math", nullptr
    },

    // -------------------------------------------------------------------------
    // FILE I/O OPERATIONS
    // -------------------------------------------------------------------------
    {
        "FILE_OPEN_READ", "_FILE_OPEN_READ", reinterpret_cast<RuntimeFunctionPtr>(FILE_OPEN_READ), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Open file for reading", "File", nullptr
    },
    {
        "FILE_OPEN_WRITE", "_FILE_OPEN_WRITE", reinterpret_cast<RuntimeFunctionPtr>(FILE_OPEN_WRITE), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Open file for writing", "File", nullptr
    },
    {
        "FILE_OPEN_APPEND", "_FILE_OPEN_APPEND", reinterpret_cast<RuntimeFunctionPtr>(FILE_OPEN_APPEND), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Open file for appending", "File", nullptr
    },
    {
        "FILE_CLOSE", "_FILE_CLOSE", reinterpret_cast<RuntimeFunctionPtr>(FILE_CLOSE), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Close file handle", "File", nullptr
    },
    {
        "FILE_WRITES", "_FILE_WRITES", reinterpret_cast<RuntimeFunctionPtr>(FILE_WRITES), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Write string to file", "File", nullptr
    },
    {
        "FILE_READS", "_FILE_READS", reinterpret_cast<RuntimeFunctionPtr>(FILE_READS), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING,
        "Read string from file", "File", nullptr
    },
    {
        "FILE_READ", "_FILE_READ", reinterpret_cast<RuntimeFunctionPtr>(FILE_READ), 3,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Read bytes from file", "File", nullptr
    },
    {
        "FILE_WRITE", "_FILE_WRITE", reinterpret_cast<RuntimeFunctionPtr>(FILE_WRITE), 3,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Write bytes to file", "File", nullptr
    },
    {
        "FILE_SEEK", "_FILE_SEEK", reinterpret_cast<RuntimeFunctionPtr>(FILE_SEEK), 3,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Seek to position in file", "File", nullptr
    },
    {
        "FILE_TELL", "_FILE_TELL", reinterpret_cast<RuntimeFunctionPtr>(FILE_TELL), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get current position in file", "File", nullptr
    },
    {
        "FILE_EOF", "_FILE_EOF", reinterpret_cast<RuntimeFunctionPtr>(FILE_EOF), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Check if at end of file", "File", nullptr
    },

    // -------------------------------------------------------------------------
    // SYSTEM FUNCTIONS
    // -------------------------------------------------------------------------
    {
        "FINISH", "_FINISH", reinterpret_cast<RuntimeFunctionPtr>(finish), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Terminate program execution", "System", nullptr
    },
    {
        "SETTYPE", "_SETTYPE", nullptr, 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Set type information (compiler intrinsic)", "System", nullptr
    },



    // -------------------------------------------------------------------------
    // HEAP MANAGER / MEMORY MANAGEMENT
    // -------------------------------------------------------------------------
    {
        "HeapManager_enter_scope", "_HeapManager_enter_scope", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_enterScope), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Enter new memory management scope", "Memory", nullptr
    },
    {
        "HeapManager_exit_scope", "_HeapManager_exit_scope", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_exitScope), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Exit current memory management scope", "Memory", nullptr
    },
    {
        "HEAPMANAGER_ISSAMMENABLED", "_HEAPMANAGER_ISSAMMENABLED", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_isSAMMEnabled), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Check if SAMM (Scope Aware Memory Management) is enabled", "Memory", nullptr
    },
    {
        "HEAPMANAGER_SETSAMMENABLED", "_HEAPMANAGER_SETSAMMENABLED", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_setSAMMEnabled), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Enable or disable SAMM", "Memory", nullptr
    },
    {
        "HEAPMANAGER_WAITFORSAMM", "_HEAPMANAGER_WAITFORSAMM", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_waitForSAMM), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Wait for SAMM operations to complete", "Memory", nullptr
    },
    {
        "OBJECT_HEAP_ALLOC", "_OBJECT_HEAP_ALLOC", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_OBJECT_HEAP_ALLOC), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Allocate object on heap", "Memory", nullptr
    },
    {
        "OBJECT_HEAP_FREE", "_OBJECT_HEAP_FREE", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_OBJECT_HEAP_FREE), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Free object from heap", "Memory", nullptr
    },
    {
        "RUNTIME_METHOD_LOOKUP", "_RUNTIME_METHOD_LOOKUP", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_RUNTIME_METHOD_LOOKUP), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Look up method at runtime", "Memory", nullptr
    },
    {
        "PIC_RUNTIME_HELPER", "_PIC_RUNTIME_HELPER", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_PIC_RUNTIME_HELPER), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Polymorphic inline cache helper", "Memory", nullptr
    },
    {
        "DEBUG_PRINT_VTABLE_STORE", "_DEBUG_PRINT_VTABLE_STORE", reinterpret_cast<RuntimeFunctionPtr>(DEBUG_PRINT_VTABLE_STORE), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Debug print vtable store verification", "Debug", nullptr
    },
    {
        "DEBUG_PRINT_FINAL_OBJECT", "_DEBUG_PRINT_FINAL_OBJECT", reinterpret_cast<RuntimeFunctionPtr>(DEBUG_PRINT_FINAL_OBJECT), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Debug print final object state", "Debug"
    }

#ifdef SDL2_RUNTIME_ENABLED
,
    // -------------------------------------------------------------------------\n    // SDL2 FUNCTIONS\n    // -------------------------------------------------------------------------
    {
        "SDL2_INIT", "_SDL2_INIT", reinterpret_cast<RuntimeFunctionPtr>(SDL2_INIT), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Initialize SDL2 video subsystem", "SDL", nullptr
    },
    {
        "SDL2_INIT_SUBSYSTEMS", "_SDL2_INIT_SUBSYSTEMS", reinterpret_cast<RuntimeFunctionPtr>(SDL2_INIT_SUBSYSTEMS), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Initialize specific SDL2 subsystems", "SDL", nullptr
    },
    {
        "SDL2_QUIT", "_SDL2_QUIT", reinterpret_cast<RuntimeFunctionPtr>(SDL2_QUIT), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Quit SDL2 and cleanup all resources", "SDL", nullptr
    },
    {
        "SDL2_CREATE_WINDOW", "_SDL2_CREATE_WINDOW", reinterpret_cast<RuntimeFunctionPtr>(SDL2_CREATE_WINDOW), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Create a 640x480 window with default settings", "SDL", nullptr
    },
    {
        "SDL2_CREATE_WINDOW_EX", "_SDL2_CREATE_WINDOW_EX", reinterpret_cast<RuntimeFunctionPtr>(SDL2_CREATE_WINDOW_EX), 6,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Create a window with custom properties", "SDL", nullptr
    },
    {
        "SDL2_DESTROY_WINDOW", "_SDL2_DESTROY_WINDOW", reinterpret_cast<RuntimeFunctionPtr>(SDL2_DESTROY_WINDOW), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Destroy a window and free its resources", "SDL", nullptr
    },
    {
        "SDL2_SET_WINDOW_TITLE", "_SDL2_SET_WINDOW_TITLE", reinterpret_cast<RuntimeFunctionPtr>(SDL2_SET_WINDOW_TITLE), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Change a window's title", "SDL", nullptr
    },
    {
        "SDL2_SET_WINDOW_SIZE", "_SDL2_SET_WINDOW_SIZE", reinterpret_cast<RuntimeFunctionPtr>(SDL2_SET_WINDOW_SIZE), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Resize a window", "SDL", nullptr
    },
    {
        "SDL2_CREATE_RENDERER", "_SDL2_CREATE_RENDERER", reinterpret_cast<RuntimeFunctionPtr>(SDL2_CREATE_RENDERER), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Create a renderer for a window with default settings", "SDL", nullptr
    },
    {
        "SDL2_CREATE_RENDERER_EX", "_SDL2_CREATE_RENDERER_EX", reinterpret_cast<RuntimeFunctionPtr>(SDL2_CREATE_RENDERER_EX), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Create a renderer with custom flags", "SDL", nullptr
    },
    {
        "SDL2_DESTROY_RENDERER", "_SDL2_DESTROY_RENDERER", reinterpret_cast<RuntimeFunctionPtr>(SDL2_DESTROY_RENDERER), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Destroy a renderer and free its resources", "SDL", nullptr
    },
    {
        "SDL2_SET_DRAW_COLOR", "_SDL2_SET_DRAW_COLOR", reinterpret_cast<RuntimeFunctionPtr>(SDL2_SET_DRAW_COLOR), 5,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Set the color for drawing operations", "SDL", nullptr
    },
    {
        "SDL2_CLEAR", "_SDL2_CLEAR", reinterpret_cast<RuntimeFunctionPtr>(SDL2_CLEAR), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Clear the screen with current draw color", "SDL", nullptr
    },
    {
        "SDL2_PRESENT", "_SDL2_PRESENT", reinterpret_cast<RuntimeFunctionPtr>(SDL2_PRESENT), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Display the rendered frame (swap buffers)", "SDL", nullptr
    },
    {
        "SDL2_DRAW_POINT", "_SDL2_DRAW_POINT", reinterpret_cast<RuntimeFunctionPtr>(SDL2_DRAW_POINT), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Draw a single point", "SDL", nullptr
    },
    {
        "SDL2_DRAW_LINE", "_SDL2_DRAW_LINE", reinterpret_cast<RuntimeFunctionPtr>(SDL2_DRAW_LINE), 5,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Draw a line between two points", "SDL", nullptr
    },
    {
        "SDL2_DRAW_RECT", "_SDL2_DRAW_RECT", reinterpret_cast<RuntimeFunctionPtr>(SDL2_DRAW_RECT), 5,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Draw a rectangle outline", "SDL", nullptr
    },
    {
        "SDL2_FILL_RECT", "_SDL2_FILL_RECT", reinterpret_cast<RuntimeFunctionPtr>(SDL2_FILL_RECT), 5,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Draw a filled rectangle", "SDL", nullptr
    },
    {
        "SDL2_POLL_EVENT", "_SDL2_POLL_EVENT", reinterpret_cast<RuntimeFunctionPtr>(SDL2_POLL_EVENT), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Check for pending events", "SDL", nullptr
    },
    {
        "SDL2_GET_EVENT_KEY", "_SDL2_GET_EVENT_KEY", reinterpret_cast<RuntimeFunctionPtr>(SDL2_GET_EVENT_KEY), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get the key code from the last keyboard event", "SDL", nullptr
    },
    {
        "SDL2_GET_EVENT_MOUSE", "_SDL2_GET_EVENT_MOUSE", reinterpret_cast<RuntimeFunctionPtr>(SDL2_GET_EVENT_MOUSE), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Get mouse coordinates from the last mouse event", "SDL", nullptr
    },
    {
        "SDL2_GET_EVENT_BUTTON", "_SDL2_GET_EVENT_BUTTON", reinterpret_cast<RuntimeFunctionPtr>(SDL2_GET_EVENT_BUTTON), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get the mouse button from the last mouse button event", "SDL", nullptr
    },
    {
        "SDL2_DELAY", "_SDL2_DELAY", reinterpret_cast<RuntimeFunctionPtr>(SDL2_DELAY), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Pause execution for specified time", "SDL", nullptr
    },
    {
        "SDL2_GET_TICKS", "_SDL2_GET_TICKS", reinterpret_cast<RuntimeFunctionPtr>(SDL2_GET_TICKS), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get milliseconds since SDL initialization", "SDL", nullptr
    },
    {
        "SDL2_GET_ERROR", "_SDL2_GET_ERROR", reinterpret_cast<RuntimeFunctionPtr>(SDL2_GET_ERROR), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING,
        "Get the last SDL error message", "SDL", nullptr
    },
    {
        "SDL2_CLEAR_ERROR", "_SDL2_CLEAR_ERROR", reinterpret_cast<RuntimeFunctionPtr>(SDL2_CLEAR_ERROR), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Clear the SDL error state", "SDL", nullptr
    },
    {
        "SDL2_GET_VERSION", "_SDL2_GET_VERSION", reinterpret_cast<RuntimeFunctionPtr>(SDL2_GET_VERSION), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get SDL2 version information", "SDL", nullptr
    },
    {
        "SDL2_GET_VIDEO_DRIVERS", "_SDL2_GET_VIDEO_DRIVERS", reinterpret_cast<RuntimeFunctionPtr>(SDL2_GET_VIDEO_DRIVERS), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get list of available video drivers", "SDL", nullptr
    },
    {
        "SDL2_GET_CURRENT_VIDEO_DRIVER", "_SDL2_GET_CURRENT_VIDEO_DRIVER", reinterpret_cast<RuntimeFunctionPtr>(SDL2_GET_CURRENT_VIDEO_DRIVER), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING,
        "Get the name of the current video driver", "SDL", nullptr
    },
    {
        "SDL2_GET_DISPLAY_MODES", "_SDL2_GET_DISPLAY_MODES", reinterpret_cast<RuntimeFunctionPtr>(SDL2_GET_DISPLAY_MODES), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get information about available display modes", "SDL", nullptr
    },
    {
        "SDL2_TEST_BASIC", "_SDL2_TEST_BASIC", reinterpret_cast<RuntimeFunctionPtr>(SDL2_TEST_BASIC), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Test basic SDL2 functionality", "SDL", nullptr
    }
#endif
    ,

    // -------------------------------------------------------------------------
    // TIMING AND PERFORMANCE METRICS
    // -------------------------------------------------------------------------
    {
        "TIMER_START", "_TIMER_START", reinterpret_cast<RuntimeFunctionPtr>(TIMER_START), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Start a timer with the given name", "TIMING", nullptr
    },
    {
        "TIMER_END", "_TIMER_END", reinterpret_cast<RuntimeFunctionPtr>(TIMER_END), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "End the most recently started timer with the given name", "TIMING", nullptr
    },
    {
        "TIMER_DISPLAY", "_TIMER_DISPLAY", reinterpret_cast<RuntimeFunctionPtr>(TIMER_DISPLAY), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Display all collected timing metrics to standard output", "TIMING", nullptr
    },
    {
        "TIMER_CLEAR", "_TIMER_CLEAR", reinterpret_cast<RuntimeFunctionPtr>(TIMER_CLEAR), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Clear all timing metrics", "TIMING", nullptr
    },
    {
        "TIMER_GET_TOTAL_NS", "_TIMER_GET_TOTAL_NS", reinterpret_cast<RuntimeFunctionPtr>(TIMER_GET_TOTAL_NS), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get total accumulated time in nanoseconds for a named timer", "TIMING", nullptr
    },
    {
        "TIMER_GET_CALL_COUNT", "_TIMER_GET_CALL_COUNT", reinterpret_cast<RuntimeFunctionPtr>(TIMER_GET_CALL_COUNT), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get the number of times a named timer has been called", "TIMING", nullptr
    },

    // === CAIRO GRAPHICS FUNCTIONS ===

    // Surface Management
    {
        "CAIRO_CREATE_SURFACE", "_CAIRO_CREATE_SURFACE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CREATE_SURFACE), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Create new Cairo drawing surface (width, height)", "GRAPHICS", cairo_create_surface_params
    },
    {
        "CAIRO_LOAD_PNG", "_CAIRO_LOAD_PNG",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_LOAD_PNG), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Load PNG image as Cairo surface", "GRAPHICS", cairo_surface_string_params
    },
    {
        "CAIRO_SAVE_PNG", "_CAIRO_SAVE_PNG",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SAVE_PNG), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Save Cairo surface to PNG file (surface, filename)", "GRAPHICS", cairo_surface_string_params
    },
    {
        "CAIRO_CLEAR", "_CAIRO_CLEAR",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CLEAR), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Clear surface with solid color (surface, rgba_color)", "GRAPHICS", cairo_surface_color_params
    },

    // Drawing Operations
    {
        "CAIRO_SET_COLOR", "_CAIRO_SET_COLOR",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SET_COLOR), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Set drawing color (surface, rgba_color)", "GRAPHICS", cairo_surface_color_params
    },
    {
        "CAIRO_FILL_RECT", "_CAIRO_FILL_RECT",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_FILL_RECT), 5,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Fill rectangle (surface, x, y, width, height)", "GRAPHICS", cairo_fill_rect_params
    },
    {
        "CAIRO_FILL_CIRCLE", "_CAIRO_FILL_CIRCLE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_FILL_CIRCLE), 4,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Fill circle (surface, cx, cy, radius)", "GRAPHICS", cairo_fill_circle_params
    },
    {
        "CAIRO_DRAW_LINE", "_CAIRO_DRAW_LINE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_LINE), 5,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Draw line between points (surface, x1, y1, x2, y2)", "GRAPHICS", cairo_draw_line_params
    },

    // Vector Operations
    {
        "CAIRO_DRAW_LINES_VEC", "_CAIRO_DRAW_LINES_VEC",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_LINES_VEC), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Draw multiple lines from vector of point pairs", "GRAPHICS", nullptr
    },
    {
        "CAIRO_FILL_RECTS_VEC", "_CAIRO_FILL_RECTS_VEC",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_FILL_RECTS_VEC), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Fill multiple rectangles from vector of rect quads", "GRAPHICS", nullptr
    },
    {
        "CAIRO_FILL_CIRCLES_VEC", "_CAIRO_FILL_CIRCLES_VEC",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_FILL_CIRCLES_VEC), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Fill multiple circles from vector of circle data", "GRAPHICS", nullptr
    },

    // Text Operations
    {
        "CAIRO_SET_FONT", "_CAIRO_SET_FONT",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SET_FONT), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Set font family and size (surface, font_name, font_size)", "GRAPHICS", cairo_set_font_params
    },
    {
        "CAIRO_DRAW_TEXT", "_CAIRO_DRAW_TEXT",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_TEXT), 4,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Draw text at position (surface, x, y, text)", "GRAPHICS", cairo_draw_text_params
    },

    // Image Operations
    {
        "CAIRO_LOAD_IMAGE", "_CAIRO_LOAD_IMAGE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_LOAD_IMAGE), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Load PNG image file", "GRAPHICS", nullptr
    },
    {
        "CAIRO_DRAW_IMAGE", "_CAIRO_DRAW_IMAGE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_IMAGE), 4,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Draw image on surface (surface, image, x, y)", "GRAPHICS", cairo_draw_image_params
    },

    // Utility Functions
    {
        "CAIRO_IS_AVAILABLE", "_CAIRO_IS_AVAILABLE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_IS_AVAILABLE), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Check if Cairo is properly initialized", "GRAPHICS", nullptr
    },
    {
        "CAIRO_GET_VERSION", "_CAIRO_GET_VERSION",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_GET_VERSION), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING,
        "Get Cairo version string", "GRAPHICS", nullptr
    },
    
    // Missing Drawing State functions
    {
        "CAIRO_SET_LINE_WIDTH", "_CAIRO_SET_LINE_WIDTH",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SET_LINE_WIDTH), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Set line width for strokes (surface, width)", "GRAPHICS", cairo_set_line_width_params
    },
    {
        "CAIRO_SET_LINE_JOIN", "_CAIRO_SET_LINE_JOIN",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SET_LINE_JOIN), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Set line join style (surface, join_style)", "GRAPHICS", cairo_surface_int_params
    },
    {
        "CAIRO_SET_LINE_CAP", "_CAIRO_SET_LINE_CAP",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SET_LINE_CAP), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Set line cap style (surface, cap_style)", "GRAPHICS", cairo_surface_int_params
    },
    {
        "CAIRO_SET_OPACITY", "_CAIRO_SET_OPACITY",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SET_OPACITY), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Set opacity for drawing operations (surface, opacity)", "GRAPHICS", cairo_set_opacity_params
    },
    {
        "CAIRO_SET_ANTIALIAS", "_CAIRO_SET_ANTIALIAS",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SET_ANTIALIAS), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Set antialiasing mode (surface, antialias_mode)", "GRAPHICS", cairo_antialias_params
    },
    {
        "CAIRO_GET_ANTIALIAS", "_CAIRO_GET_ANTIALIAS",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_GET_ANTIALIAS), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get current antialiasing mode (surface)", "GRAPHICS", cairo_surface_handle_only_params
    },

    // Missing Basic Shapes functions
    {
        "CAIRO_STROKE_RECT", "_CAIRO_STROKE_RECT",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_STROKE_RECT), 5,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Stroke rectangle outline (surface, x, y, width, height)", "GRAPHICS", cairo_stroke_rect_params
    },
    {
        "CAIRO_STROKE_CIRCLE", "_CAIRO_STROKE_CIRCLE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_STROKE_CIRCLE), 4,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Stroke circle outline (surface, cx, cy, radius)", "GRAPHICS", cairo_stroke_circle_params
    },
    {
        "CAIRO_DRAW_POINT", "_CAIRO_DRAW_POINT",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_POINT), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Draw single point (surface, x, y)", "GRAPHICS", cairo_draw_point_params
    },

    // Vector Operations functions (duplicates removed)
    {
        "CAIRO_DRAW_POLYLINE", "_CAIRO_DRAW_POLYLINE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_POLYLINE), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Draw polyline from vector (surface, points, close_path)", "GRAPHICS", cairo_polyline_params
    },
    {
        "CAIRO_FILL_POLYGON", "_CAIRO_FILL_POLYGON",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_FILL_POLYGON), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Fill polygon from vector (surface, points)", "GRAPHICS", cairo_surface_vector_params
    },

    // Path Operations functions
    {
        "CAIRO_BEGIN_PATH", "_CAIRO_BEGIN_PATH",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_BEGIN_PATH), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Begin new path (surface)", "GRAPHICS", cairo_surface_handle_only_params
    },
    {
        "CAIRO_MOVE_TO", "_CAIRO_MOVE_TO",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_MOVE_TO), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Move to point without drawing (surface, x, y)", "GRAPHICS", cairo_draw_point_params
    },
    {
        "CAIRO_LINE_TO", "_CAIRO_LINE_TO",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_LINE_TO), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Line to point from current position (surface, x, y)", "GRAPHICS", cairo_draw_point_params
    },
    {
        "CAIRO_CURVE_TO", "_CAIRO_CURVE_TO",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CURVE_TO), 7,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Draw cubic Bezier curve (surface, x1, y1, x2, y2, x3, y3)", "GRAPHICS", cairo_curve_to_params
    },
    {
        "CAIRO_CLOSE_PATH", "_CAIRO_CLOSE_PATH",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CLOSE_PATH), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Close current path (surface)", "GRAPHICS", cairo_surface_handle_only_params
    },
    {
        "CAIRO_FILL_PATH", "_CAIRO_FILL_PATH",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_FILL_PATH), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Fill current path (surface)", "GRAPHICS", cairo_surface_handle_only_params
    },
    {
        "CAIRO_STROKE_PATH", "_CAIRO_STROKE_PATH",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_STROKE_PATH), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Stroke current path (surface)", "GRAPHICS", cairo_surface_handle_only_params
    },

    // Text Operations functions
    {
        "CAIRO_TEXT_SIZE", "_CAIRO_TEXT_SIZE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_TEXT_SIZE), 4,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Get text dimensions (surface, text, width_ptr, height_ptr)", "GRAPHICS", nullptr
    },
    {
        "CAIRO_DRAW_TEXT_COLORED", "_CAIRO_DRAW_TEXT_COLORED",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_TEXT_COLORED), 5,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Draw text with specific color (surface, x, y, text, rgba_color)", "GRAPHICS", cairo_draw_text_colored_params
    },

    // Image Operations functions (CAIRO_LOAD_IMAGE already registered above)
    {
        "CAIRO_DRAW_IMAGE_SCALED", "_CAIRO_DRAW_IMAGE_SCALED",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_IMAGE_SCALED), 6,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Draw image with scaling (surface, image, x, y, scale_x, scale_y)", "GRAPHICS", cairo_draw_image_scaled_params
    },
    {
        "CAIRO_DRAW_IMAGE_ROTATED", "_CAIRO_DRAW_IMAGE_ROTATED",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_IMAGE_ROTATED), 5,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Draw image with rotation (surface, image, x, y, angle)", "GRAPHICS", cairo_draw_image_rotated_params
    },
    {
        "CAIRO_IMAGE_SIZE", "_CAIRO_IMAGE_SIZE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_IMAGE_SIZE), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Get image dimensions (image, width_ptr, height_ptr)", "GRAPHICS", nullptr
    },

    // Transformation Operations functions
    {
        "CAIRO_SAVE", "_CAIRO_SAVE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SAVE), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Save current transformation state (surface)", "GRAPHICS", cairo_surface_handle_only_params
    },
    {
        "CAIRO_RESTORE", "_CAIRO_RESTORE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_RESTORE), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Restore previous transformation state (surface)", "GRAPHICS", cairo_surface_handle_only_params
    },
    {
        "CAIRO_TRANSLATE", "_CAIRO_TRANSLATE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_TRANSLATE), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Translate coordinate system (surface, tx, ty)", "GRAPHICS", cairo_translate_params
    },
    {
        "CAIRO_SCALE", "_CAIRO_SCALE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SCALE), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Scale coordinate system (surface, sx, sy)", "GRAPHICS", cairo_scale_params
    },
    {
        "CAIRO_ROTATE", "_CAIRO_ROTATE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_ROTATE), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Rotate coordinate system (surface, angle)", "GRAPHICS", cairo_rotate_params
    },

    // Surface Management functions
    {
        "CAIRO_GET_SIZE", "_CAIRO_GET_SIZE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_GET_SIZE), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Get surface dimensions (surface, width_ptr, height_ptr)", "GRAPHICS", nullptr
    },
    {
        "CAIRO_CLONE_SURFACE", "_CAIRO_CLONE_SURFACE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CLONE_SURFACE), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Clone surface for double-buffering", "GRAPHICS", cairo_surface_handle_only_params
    },

    // Resource Management functions
    {
        "CAIRO_RELEASE_SURFACE", "_CAIRO_RELEASE_SURFACE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_RELEASE_SURFACE), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Manually release surface (surface)", "GRAPHICS", cairo_surface_handle_only_params
    },
    {
        "CAIRO_RELEASE_IMAGE", "_CAIRO_RELEASE_IMAGE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_RELEASE_IMAGE), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Manually release image (image)", "GRAPHICS", cairo_image_handle_only_params
    },
    {
        "CAIRO_SURFACE_COUNT", "_CAIRO_SURFACE_COUNT",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SURFACE_COUNT), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get number of active surfaces", "GRAPHICS", nullptr
    },
    {
        "CAIRO_IMAGE_COUNT", "_CAIRO_IMAGE_COUNT",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_IMAGE_COUNT), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get number of active images", "GRAPHICS", nullptr
    },

    // Utility functions (duplicates removed - already registered above)
    {
        "CAIRO_GET_ERROR", "_CAIRO_GET_ERROR",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_GET_ERROR), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING,
        "Get last Cairo error message", "GRAPHICS", nullptr
    },
    {
        "CAIRO_CLEAR_ERROR", "_CAIRO_CLEAR_ERROR",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CLEAR_ERROR), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Clear any pending Cairo error", "GRAPHICS", nullptr
    },

    // SAMM integration functions
    {
        "CAIRO_CREATE_SURFACE_RETAINED", "_CAIRO_CREATE_SURFACE_RETAINED",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CREATE_SURFACE_RETAINED), 3,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Create surface retained in parent scope (width, height, parent_scope_offset)", "GRAPHICS", cairo_retained_surface_params
    },
    {
        "CAIRO_LOAD_PNG_RETAINED", "_CAIRO_LOAD_PNG_RETAINED",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_LOAD_PNG_RETAINED), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Load PNG retained in parent scope (filename, parent_scope_offset)", "GRAPHICS", cairo_retained_png_params
    },
    {
        "CAIRO_LOAD_IMAGE_RETAINED", "_CAIRO_LOAD_IMAGE_RETAINED",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_LOAD_IMAGE_RETAINED), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Load image retained in parent scope (filename, parent_scope_offset)", "GRAPHICS", cairo_retained_image_params
    },
    {
        "CAIRO_SAMM_SCOPE_DEPTH", "_CAIRO_SAMM_SCOPE_DEPTH",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SAMM_SCOPE_DEPTH), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get current SAMM scope depth", "GRAPHICS", nullptr
    },
    {
        "CAIRO_SAMM_FORCE_CLEANUP", "_CAIRO_SAMM_FORCE_CLEANUP",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SAMM_FORCE_CLEANUP), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Force cleanup of Cairo resources", "GRAPHICS", nullptr
    },
    {
        "CAIRO_SAMM_GET_STATS", "_CAIRO_SAMM_GET_STATS",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SAMM_GET_STATS), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Get SAMM statistics (active_surfaces_ptr, active_images_ptr, memory_usage_ptr)", "GRAPHICS", nullptr
    },
    {
        "CAIRO_SAMM_SET_TRACE", "_CAIRO_SAMM_SET_TRACE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SAMM_SET_TRACE), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Enable/disable SAMM debug tracing (enabled)", "GRAPHICS", cairo_surface_handle_only_params
    },
    {
        "CAIRO_SAMM_DUMP_STATE", "_CAIRO_SAMM_DUMP_STATE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SAMM_DUMP_STATE), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Dump SAMM state for debugging", "GRAPHICS", nullptr
    },

    // SDL2 Display Integration functions
    {
        "CAIRO_DISPLAY_SDL", "_CAIRO_DISPLAY_SDL",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DISPLAY_SDL), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Display Cairo surface in SDL2 window (surface, sdl_window_id)", "GRAPHICS", cairo_sdl_display_params
    },
    {
        "CAIRO_CREATE_SDL_WINDOW", "_CAIRO_CREATE_SDL_WINDOW",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CREATE_SDL_WINDOW), 3,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Create SDL2 window for Cairo display (title, width, height)", "GRAPHICS", cairo_sdl_window_params
    },
    {
        "CAIRO_UPDATE_SDL_WINDOW", "_CAIRO_UPDATE_SDL_WINDOW",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_UPDATE_SDL_WINDOW), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Update SDL2 window with Cairo surface (surface, sdl_window_id)", "GRAPHICS", cairo_sdl_display_params
    },
    {
        "CAIRO_TO_SDL_TEXTURE", "_CAIRO_TO_SDL_TEXTURE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_TO_SDL_TEXTURE), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Convert Cairo surface to SDL2 texture (surface, sdl_renderer_id)", "GRAPHICS", cairo_sdl_display_params
    }
};

// Calculate manifest size
static const int g_manifest_count = sizeof(g_runtime_manifest) / sizeof(g_runtime_manifest[0]);

// =============================================================================
// API IMPLEMENTATION FUNCTIONS
// =============================================================================

const RuntimeFunctionDescriptor* get_runtime_manifest(int& count) {
    count = g_manifest_count;
    return g_runtime_manifest;
}

const char* get_runtime_api_version() {
    return "1.0.0";
}

void initialize_runtime_system() {
    // Initialize any global runtime state here
    // This replaces scattered initialization calls
}

void shutdown_runtime_system() {
    // Clean shutdown of runtime resources
}

bool verify_runtime_manifest() {
    // Verify all function pointers are non-null (except SETTYPE which is a compiler intrinsic)
    for (int i = 0; i < g_manifest_count; ++i) {
        const auto& desc = g_runtime_manifest[i];
        // SETTYPE is special - it's a compiler intrinsic with null function pointer
        if (!desc.function_pointer && strcmp(desc.veneer_name, "SETTYPE") != 0) {
            return false;
        }
        if (!desc.veneer_name || !desc.linker_symbol || !desc.description) {
            return false;
        }
        if (desc.arg_count < 0) {
            return false;
        }
    }
    return true;
}

const RuntimeFunctionDescriptor* lookup_runtime_function(const char* veneer_name) {
    if (!veneer_name) return nullptr;
    
    for (int i = 0; i < g_manifest_count; ++i) {
        if (strcmp(g_runtime_manifest[i].veneer_name, veneer_name) == 0) {
            return &g_runtime_manifest[i];
        }
    }
    return nullptr;
}

const RuntimeFunctionDescriptor** get_functions_by_category(const char* category, int& count) {
    static const RuntimeFunctionDescriptor* category_functions[256]; // Static buffer
    count = 0;
    
    if (!category) return nullptr;
    
    for (int i = 0; i < g_manifest_count && count < 255; ++i) {
        if (strcmp(g_runtime_manifest[i].category, category) == 0) {
            category_functions[count++] = &g_runtime_manifest[i];
        }
    }
    
    return category_functions;
}
