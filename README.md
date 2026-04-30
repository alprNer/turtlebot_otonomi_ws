# TurtleBot Otonom Haritalama ve Navigasyon Sistemi

Bu çalışma alanı (`turtlebot_otonomi_ws`), TurtleBot3 simülasyon ortamında robotun önce çevresini otonom olarak keşfedip haritalandırmasını, ardından kaydedilen bu harita üzerinden belirlenen noktalara otonom navigasyon gerçekleştirmesini sağlar.

## 📋 Gereksinimler

Sistemin çalışması için aşağıdaki paketlerin yüklü olması gerekmektedir:

*   **Ubuntu 20.04** ve **ROS Noetic**
*   **TurtleBot3 Simülasyon Paketleri:** `turtlebot3_gazebo`, `turtlebot3_slam`, `turtlebot3_navigation`, `turtlebot3_bringup`
*   **Bağımlılıklar:** `gmapping`, `map_server`, `move_base`, `amcl`

Kurulum için şu komutları kullanabilirsiniz:
```bash
sudo apt update
sudo apt install ros-noetic-turtlebot3-msgs ros-noetic-turtlebot3-simulations ros-noetic-gmapping ros-noetic-navigation
```

## 🛠️ Kurulum

1.  Çalışma alanını derleyin:
    ```bash
    cd ~/turtlebot_otonomi_ws
    catkin_make
    source devel/setup.bash
    ```
2.  Kullanılacak robot modelini (burger veya waffle) tanımlayın:
    ```bash
    export TURTLEBOT3_MODEL=waffle
    ```

## 🚀 Kullanım

Sistem iki ana aşamadan oluşmaktadır:

### 1. Aşama: Otonom Haritalama (SLAM)
Bu aşamada robot, Lidar verilerini kullanarak engellerden kaçar ve çevresini keşfeder. `otonom_beyin` düğümü haritanın tamamlandığını algıladığında haritayı otomatik olarak `$HOME/benim_haritam` adıyla kaydeder.

**Başlatmak için:**
```bash
roslaunch turtlebot_otonomi haritalama.launch
```
*   **İşleyiş:** `turtlebot3_world.launch` ve `gmapping` ile SLAM başlatılır. `kasif_node` ve `otonom_beyin` düğümleri keşif sürecini yönetir.

### 2. Aşama: Otonom Navigasyon (Hedefe Gitme)
Harita oluşturulduktan sonra robotun bu harita üzerinde belirlenen 5 koordinat noktasına sırasıyla gitmesi sağlanır.

**Başlatmak için:**
```bash
roslaunch turtlebot_otonomi nav.launch
```
*   **İşleyiş:** `map_server` üzerinden harita yüklenir, AMCL ve Move Base ile konumlandırma ve rota planlama yapılır. `nav_hedef_node` robotu tanımlı 5 noktaya sırayla ulaştırır.

## 📂 Dosya Yapısı

*   **`haritalama.launch`**: Gazebo, SLAM ve keşif düğümlerini koordine eder.
*   **`nav.launch`**: Harita sunucusu, AMCL, Move Base ve RViz'i başlatır.
*   **`kasif_node.cpp`**: Lidar verisiyle engellerden kaçınarak otonom gezinmeyi sağlar.
*   **`otonom_beyin.cpp`**: Harita alanını izler ve keşif bittiğinde haritayı kaydeder.
*   **`nav_hedef.cpp`**: Önceden tanımlanmış 5 farklı hedef koordinatına navigasyonu yönetir.
```
