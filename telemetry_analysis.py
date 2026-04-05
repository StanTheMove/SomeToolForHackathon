# Підключаємо бібліотеки
import streamlit as st          # створює веб-сторінку у браузері
import numpy as np              # математичні операції
import pandas as pd             # працює з таблицями даних
from pymavlink import mavutil   # читає бінарні .BIN файли від дрона
# Читає .BIN файл і повертає таблицю з GPS даними
def load_bin_file(uploaded_file):
    import tempfile, os # створюємо тимчасовий файл на диску щоб прочитати його
    with tempfile.NamedTemporaryFile(delete=False, suffix='.BIN') as tmp:
        tmp.write(uploaded_file.read()) # записуємо байти у тимчасовий файл
        tmp_path = tmp.name # запам'ятовуємо шлях до файлу
    log = mavutil.mavlink_connection(tmp_path) # відкриваємо бінарний файл
    gps_data = [] # порожня коробка для точок
    while True:
        msg = log.recv_match(type='GPS', blocking=False)
        if msg is None:
            break # Читаємо GPS-повідомлення, якщо їх більше нема — виходимо
        if msg.Status >= 3:  # сигнал GPS надійний
            gps_data.append({
                'lat': msg.Lat, # широта
                'lon': msg.Lng, # довгота
                'alt': msg.Alt, # висота
                'spd': msg.Spd, # швидкість в м/с
                'time_us': msg.TimeUS # час 
            })
    os.unlink(tmp_path) # видаляємо тимчасовий файл
    return pd.DataFrame(gps_data) # повертаємо таблицю
# Рахує відстань між двома GPS точками по поверхні Землі
def haversine(lat1, lon1, lat2, lon2):
    R = 6378137.0 # реальний радіус Землі в метрах
    dlat = np.radians(lat2 - lat1)
    dlon = np.radians(lon2 - lon1)
    a = np.sin(dlat/2)**2 + np.cos(np.radians(lat1)) * np.cos(np.radians(lat2)) * np.sin(dlon/2)**2
    return R * 2 * np.arcsin(np.sqrt(a)) # результат у метрах
# Складає відстані між усіма сусідніми точками (загальна дистанція)
def total_distance(df):
    dist = 0
    for i in range(1, len(df)):
        dist += haversine(
            df['lat'].iloc[i-1], df['lon'].iloc[i-1], # попередня точка
            df['lat'].iloc[i],   df['lon'].iloc[i] # поточна точка
        )
    return dist
# Налаштування сторінки
st.set_page_config(page_title="Метрики польоту БПЛА", layout="wide")
st.title("Метрики польоту БПЛА")
st.write("Завантажте .BIN файл для аналізу")
# Кнопка завантаження файлу
uploaded_file = st.file_uploader("Оберіть .BIN файл", type=["BIN"])

if uploaded_file is not None:
    st.success(f"Файл завантажено: {uploaded_file.name}")
    with st.spinner("Читаємо дані..."):
        df = load_bin_file(uploaded_file)
    if df.empty:
        st.error("Не вдалось прочитати GPS дані")
    else: # Рахуємо загальну дистанцію через haversine
        dist_m = total_distance(df)
        # Рахуємо тривалість польоту
        duration_s = (df['time_us'].iloc[-1] - df['time_us'].iloc[0]) / 1_000_000 # Різниця між останнім і першим часом — ділимо на 1,000,000 тому що час у мікросекундах
        minutes = int(duration_s // 60) # хвилини
        seconds = int(duration_s % 60)  # секунди

        st.subheader("Показники польоту")
        col1, col2, col3, col4, col5, col6 = st.columns(6)
        col1.metric("Макс. швидкість", f"{df['spd'].max():.1f} м/с")
        col2.metric("Макс. висота",    f"{df['alt'].max():.1f} м")
        col3.metric("Дистанція",       f"{dist_m:.0f} м")
        col4.metric("Тривалість",      f"{minutes}:{seconds:02d}")
        col5.metric("Перепад висоти",  f"{df['alt'].max() - df['alt'].min():.1f} м")
        max_acceleration = 0.0 
        col6.metric("Макс. прискорення", f"{max_acceleration:.1f} м/с²")
        # Таблиця з усіма GPS даними
        st.subheader("Таблиця GPS даних")
        st.dataframe(df)

        ## streamlit run telemetry_analysis.py
