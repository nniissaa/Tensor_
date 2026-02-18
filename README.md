#  Dinamik Tensör Yönetimi ve Quantization

Bu proje, RAM (bellek) kısıtlı gömülü sistemler (Arduino, ESP32 vb.) üzerinde yapay zeka (TinyML) modellerinin çalıştırılabilmesi için optimize edilmiş bir dinamik tensör mimarisinin C dili ile sıfırdan implementasyonudur.

## 🎯 Projenin Amacı ve Mimari Çözüm
Standart makine öğrenmesi modelleri 32-bit yüksek hassasiyetli (Float32) matrisler kullanır. Ancak donanım kaynakları sınırlı sistemlerde bu durum bellek aşımına (Out of Memory) sebep olur. 

Bu projede, harici bir matematik kütüphanesi kullanılmadan C dilinin temel (primitive) yetenekleri kullanılarak şu çözümler üretilmiştir:
* **Union ile Bellek Optimizasyonu:** `struct` içerisinde `union` kullanılarak Float32, Float16 ve Int8 pointer'ları aynı bellek adresini paylaşacak şekilde tasarlanmıştır. Bu sayede kullanılmayan veri tipleri için RAM'de yer işgal edilmez.
* **1D Bellekte 2D Matris Yönetimi:** Ayrılan tek boyutlu ardışık bellek bloğu, pointer matematiği (`i * cols + j`) kullanılarak 2 boyutlu bir matris gibi yönetilmiştir.
* **Kuantizasyon (Quantization) & Clamping:** Float32 formatındaki matris verileri, bellekten **%75 oranında tasarruf** sağlamak amacıyla 8-bit Integer (Int8) formatına asimetrik olarak kuantize edilmiş ve bellek taşmalarını önlemek için değerler -128 ile 127 arasına hapsedilmiştir (Clamping).

## 🛠️ Kullanılan Teknolojiler ve Yöntemler
* **Programlama Dili:** C (Standart kütüphaneler: `stdio.h`, `stdlib.h`, `stdint.h`)
* **Geliştirme Ortamı (IDE):** XCode
* **Geliştirme Yaklaşımı:** Agentic Kodlama (Proje mimarisi ve debug süreçleri Gemini / ChatGPT  dil modelleri ile iteratif prompt mühendisliği yapılarak kurgulanmıştır.)

## 🚀 Kurulum ve Çalıştırma
Projeyi kendi yerel ortamınızda derlemek ve çalıştırmak için bir C derleyicisine (GCC vb.) ihtiyacınız vardır.

```bash
# Repoyu klonlayın
git clone [https://github.com/KULLANICI_ADINIZ/TinyML-Dynamic-Tensor-C.git](https://github.com/KULLANICI_ADINIZ/TinyML-Dynamic-Tensor-C.git)
cd TinyML-Dynamic-Tensor-C

# Kodu derleyin
gcc main.c -o tensor_app

# Uygulamayı çalıştırın
./tensor_app
