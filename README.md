09D
HELLOBEST2026
FLYHIGH
TELEMETRY
VIBE




# UAV Telemetry Tool

Інструмент для аналізу польотних логів дронів (Ardupilot `.BIN` файли). C++ бекенд читає сирі бінарні дані, рахує метрики і передає їх Python фронтенду для візуалізації.

---

## Як це працює — повний pipeline

```
gps.bin + imu.bin
       ↓
  C++ main.exe
  (читає байти, рахує метрики)
       ↓
  metrics.json
       ↓
  dashboard.py        telemetry_analysis.py
  (3D траєкторія)     (таблиця метрик через pymavlink)
```

1. C++ програма читає `gps.bin` і `imu.bin` через `BIN_READER`
2. `Parser_for_BIN_class` парсить байти у структури GPS і IMU
3. `TelemetryAnalyzer` рахує всі метрики польоту
4. Результат записується у `metrics.json`
5. `dashboard.py` читає `metrics.json` і будує 3D траєкторію
6. `telemetry_analysis.py` — окремий інструмент, читає `.BIN` напряму через `pymavlink`

---

## Бекенд — C++

### Читання бінарних файлів

Клас `BIN_READER` відкриває бінарний файл і читає сирі байти:

```cpp
BIN_READER reader_gps("gps.bin");
BIN_READER reader_imu("imu.bin");
```

Перевірка що файл відкрився:
```cpp
if (!reader_gps.isOpen()) {
    cout << "do not open gps file\n";
    return 1;
}
```

### Структури даних

Для зберігання розпарсених даних:

```cpp
struct GPS_container {
    float latitude;
    float longitude;
    float altitude;
};

struct IMU_container {
    float accelX;
    float accelY;
    float accelZ;
};

struct Data {
    vector<GPS_container> gps;
    vector<IMU_container> imu;
};
```

`#pragma pack(push, 1)` — вирівнювання структур точно відповідає бінарному формату файлу без зайвих байтів.

### Парсинг

`Parser_for_BIN_class::parse()` читає байти з файлу і заповнює структури:

```cpp
Parser_for_BIN_class::parse(reader_gps, storage);
Parser_for_BIN_class::parse(reader_imu, storage);
```

### Розрахунок метрик — TelemetryAnalyzer

`TelemetryAnalyzer` приймає масиви GPS і IMU точок і повертає `FlightMetrics`:

```cpp
TelemetryAnalyzer analyzer;
FlightMetrics metrics = analyzer.analyzeTelemetry(storage.gps, storage.imu);
```

**Що рахується:**

| Метрика | Метод |
|---|---|
| Загальна дистанція | Haversine між усіма сусідніми GPS точками |
| Тривалість польоту | Різниця першого і останнього timestamp |
| Максимальна швидкість | З GPS даних |
| Максимальне прискорення | Трапецієвидне інтегрування IMU |

**Haversine** — точний розрахунок відстані між двома GPS точками з урахуванням кривизни Землі:
```
a = sin²(Δlat/2) + cos(lat₁)·cos(lat₂)·sin²(Δlon/2)
d = R · 2 · arcsin(√a),  де R = 6 378 137 м
```

**Трапецієвидне інтегрування** — отримання швидкості з прискорень IMU:
```
v(t) = v₀ + Σ [(a_i + a_{i+1}) / 2 · Δt]
```

### Генерація metrics.json

C++ програма записує всі метрики і GPS точки у JSON файл який потім читає Python:

```json
{
  "total_distance": 1500.0,
  "flight_duration": 120.0,
  "max_speed": 15.3,
  "max_acceleration": 3.2,
  "gps_points": [
    {"lat": 49.123, "lon": 24.456, "alt": 150.0},
    ...
  ]
}
```

---

## Бекенд — Python (telemetry_analysis.py)

Окремий інструмент що читає `.BIN` файл напряму через `pymavlink`. Фільтрує тільки надійні GPS пакети (`Status >= 3`) і будує `pandas DataFrame`:

```python
msg = log.recv_match(type='GPS', blocking=False)
if msg.Status >= 3:
    gps_data.append({'lat': msg.Lat, 'lon': msg.Lng,
                     'alt': msg.Alt, 'spd': msg.Spd, 'time_us': msg.TimeUS})
```

---

## Фронтенд — Python

### dashboard.py — 3D візуалізація

Читає `metrics.json`, конвертує GPS координати з градусів у метри (WGS-84 → ENU) і будує інтерактивний 3D графік:

```python
def wgs84_to_enu(lat, lon, alt, lat0, lon0, alt0):
    R = 6378137.0
    east  = R * np.radians(lon - lon0) * np.cos(np.radians(lat0))
    north = R * np.radians(lat - lat0)
    up    = alt - alt0
    return east, north, up
```

- колорування траєкторії залежить від висоти (палітра Inferno)
- зелена точка — старт, червона — фініш
- реальні пропорції без спотворень (`aspectmode='data'`)

### telemetry_analysis.py — метрики

Streamlit застосунок з таблицею GPS даних і картками метрик.

---

## Запуск

### Крок 1 — Зібрати і запустити C++ бекенд

```bash
# Windows (MinGW)
g++ main.cpp bin_reader.cpp -o main.exe
main.exe
```

Після запуску у папці з'явиться `metrics.json`.

### Крок 2 — Запустити Python

```bash
pip install streamlit pymavlink pandas numpy plotly

# 3D візуалізація (читає metrics.json)
streamlit run dashboard.py

# або аналіз метрик напряму з .BIN
streamlit run telemetry_analysis.py
```

Відкрити в браузері: **http://localhost:8501**

---

### Docker

```bash
docker build -t uav-tool .
docker run -p 8501:8501 uav-tool
```

**Dockerfile:**
```dockerfile
FROM python:3.11-slim
WORKDIR /app
COPY . .
RUN pip install streamlit pymavlink pandas numpy plotly
EXPOSE 8501
CMD ["streamlit", "run", "dashboard.py", "--server.port=8501", "--server.address=0.0.0.0"]
```

---

## Структура репозиторію

```
SomeToolForHackathon/
├── main.cpp                  # точка входу C++ бекенду
├── BIN_class_struct.hpp      # клас BIN_READER
├── bin_reader.cpp            # реалізація читання байтів
├── Parser_for_BIN_class.hpp  # парсер бінарного формату
├── TelemetryAnalyzer.hpp     # розрахунок метрик
├── Data.hpp                  # структури GPS/IMU/FlightMetrics
├── telemetry_analysis.py     # Python аналіз через pymavlink
├── dashboard.py              # 3D візуалізація траєкторії
└── README.md
```

---

## Залежності

**C++:** g++ / MinGW, стандарт C++17

**Python:**
```
streamlit
pymavlink
pandas
numpy
plotly
```
