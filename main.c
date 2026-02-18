#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// 1. ADIM: TIP TANIMLARI
typedef enum {
    TYPE_F32 = 0,
    TYPE_F16 = 1,
    TYPE_I8  = 2
} TensorType;

typedef struct {
    TensorType type;
    int rows; // "Matris Yonetimi" kriteri icin
    int cols; // "Matris Yonetimi" kriteri icin
    union {
        float* f32;     // 32-bit: Yuksek Hassasiyet
        int16_t* f16;   // 16-bit: Orta Hassasiyet (Half-precision simülasyonu)
        int8_t* i8;     // 8-bit: Dusuk Hassasiyet (Quantized)
    } data;
} Tensor;

// 2. ADIM: MATRIS BELLEK YONETIMI
Tensor create_tensor(TensorType type, int rows, int cols) {
    Tensor t;
    t.type = type;
    t.rows = rows;
    t.cols = cols;
    
    int total_elements = rows * cols; // Matrisin toplam eleman sayisi
    
    // Her tipin byte boyutunu ayrintili hesapla
    size_t element_size = (type == TYPE_F32) ? sizeof(float) :
                          (type == TYPE_F16) ? sizeof(int16_t) : sizeof(int8_t);

    // GOMULU SISTEM DISIPLINI: Bellek tahsisi ve NULL kontrolu
    void* mem = malloc(total_elements * element_size);
    if (mem == NULL) {
        fprintf(stderr, "Kritik Hata: Bellek ayrilamadi!\n");
        exit(EXIT_FAILURE);
    }
    
    // Union icindeki dogru pointer'a adres atamasi
    if (type == TYPE_F32) t.data.f32 = (float*)mem;
    else if (type == TYPE_F16) t.data.f16 = (int16_t*)mem;
    else t.data.i8 = (int8_t*)mem;
    
    return t;
}

// 3. ADIM: AYRINTILI QUANTIZATION (NICEMLEME) VE MATRIS YONETIMI
void quantize_to_i8(Tensor* src, Tensor* dest) {
    if (src->type != TYPE_F32 || dest->type != TYPE_I8) return;
    
    // Matris Yonetimi: 1 Boyutlu bellek dizisinde 2 Boyutlu (Satir x Sutun) islem yapma
    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            // Indeks Formulu: (satir_no * toplam_sutun) + sutun_no
            int index = i * src->cols + j;
            
            
            float val = src->data.f32[index] * 127.0f;
            
            // Clamping: Degerin 8-bit sinirlari (-128 ile 127) disina cikmasini engeller
            if (val > 127.0f) val = 127.0f;
            if (val < -128.0f) val = -128.0f;
            
            dest->data.i8[index] = (int8_t)val;
        }
    }
}

// 4. ADIM: TEKNIK RAPORLAMA VE 2D MATRIS YAZDIRMA
void print_technical_report(Tensor* t, const char* label) {
    size_t b_per_e = (t->type == TYPE_F32) ? 4 : (t->type == TYPE_F16 ? 2 : 1);
    int total_elements = t->rows * t->cols;
    
    printf(">> [%s] RAPORU\n", label);
    printf("   - Veri Tipi    : %s\n", (t->type == TYPE_F32 ? "Float32" : (t->type == TYPE_F16 ? "Float16 (int16_t Simule)" : "Int8 (Quantized)")));
    printf("   - Matris Boyutu: %dx%d (%d eleman)\n", t->rows, t->cols, total_elements);
    printf("   - Toplam Bellek: %zu Byte\n", total_elements * b_per_e);
    printf("   - Matris Gorunumu:\n");
    
    // Matrisin satir ve sutunlarini gorsel olarak dogru yazdirma
    for (int i = 0; i < t->rows; i++) {
        printf("       [ ");
        for (int j = 0; j < t->cols; j++) {
            int index = i * t->cols + j;
            
            if (t->type == TYPE_F32) printf("%7.2f ", t->data.f32[index]);
            else if (t->type == TYPE_F16) printf("%7d ", t->data.f16[index]);
            else printf("%7d ", t->data.i8[index]);
        }
        printf("]\n");
    }
    printf("\n");
}

int main() {
   
    int rows = 2;
    int cols = 3;
    
    printf("--- TINYML DINAMIK TENSOR VE MATRIS YONETIMI ---\n\n");

    // Orijinal Veri Seti (Orn: 2x3 bir sensor veya agirlik matrisi)
    Tensor f32_t = create_tensor(TYPE_F32, rows, cols);
    
    // 2D Matris verilerini atama
    float sensor_data[6] = {
        0.15f, -0.88f, 1.20f,  // 1. Satir
       -0.05f,  0.50f, 0.95f   // 2. Satir
    };
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int index = i * cols + j;
            f32_t.data.f32[index] = sensor_data[index];
        }
    }

    // Donusum Tensorleri (Matrisleri)
    Tensor f16_t = create_tensor(TYPE_F16, rows, cols);
    Tensor i8_t = create_tensor(TYPE_I8, rows, cols);

    // 16-bit Donusum Islemi (2D Matris uzerinde)
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int index = i * cols + j;
            f16_t.data.f16[index] = (int16_t)(f32_t.data.f32[index] * 1000);
        }
    }
    
    // 8-bit Quantization Islemi
    quantize_to_i8(&f32_t, &i8_t);

    // Teknik Sunum
    print_technical_report(&f32_t, "ORIJINAL YUKSEK HASSASIYET");
    print_technical_report(&f16_t, "HALF-PRECISION MATRIS");
    print_technical_report(&i8_t, "QUANTIZED MATRIS (MAX TASARRUF)");

    // MATRIS BELLEK TEMIZLIGI (Memory Deallocation)
    free(f32_t.data.f32);
    free(f16_t.data.f16);
    free(i8_t.data.i8);

    return 0;
}
