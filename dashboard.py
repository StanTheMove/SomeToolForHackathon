# Підключаємо бібліотеки
import streamlit as st          # створює веб-сторінку у браузері
import plotly.graph_objects as go  # інтерактивні 3D графіки
import numpy as np              # математичні операції
import pandas as pd             # працює з таблицями даних
from pymavlink import mavutil   # читає бінарні .BIN файли від дрона
# Читає .BIN файл і повертає таблицю з GPS даними
def load_bin_file(uploaded_file):
    import tempfile, os  # створюємо тимчасовий файл на диску щоб прочитати його
    with tempfile.NamedTemporaryFile(delete=False, suffix='.BIN') as tmp:
        tmp.write(uploaded_file.read()) # записуємо байти у тимчасовий файл
        tmp_path = tmp.name  # запам'ятовуємо шлях до файлу
    log = mavutil.mavlink_connection(tmp_path) # відкриваємо бінарний файл
    gps_data = [] # порожня коробка для точок
    while True:
        msg = log.recv_match(type='GPS', blocking=False)
        if msg is None:
            break # Читаємо GPS-повідомлення, якщо їх більше нема — виходимо
        if msg.Status >= 3: # сигнал GPS надійний
            gps_data.append({
                'lat': msg.Lat,  # широта
                'lon': msg.Lng,  # довгота
                'alt': msg.Alt,  # висота
                'spd': msg.Spd,  # швидкість в м/с
                'time_us': msg.TimeUS # час 
            })
    os.unlink(tmp_path) # видаляємо тимчасовий файл
    return pd.DataFrame(gps_data) # повертаємо таблицю 
# Переводить GPS градуси у метри від точки старту, WGS-84 у ENU
def wgs84_to_enu(lat, lon, alt, lat0, lon0, alt0):
    R = 6378137.0 # радіус Землі
    east  = R * np.radians(lon - lon0) * np.cos(np.radians(lat0)) # метри на схід
    north = R * np.radians(lat - lat0) # метри на північ
    up    = alt - alt0 # метри вгору
    return east, north, up
# Створюємо сторінку
st.set_page_config(page_title="3D Траєкторія БПЛА", layout="wide")
st.title("3D Траєкторія польоту БПЛА")
st.write("Завантажте .BIN файл для візуалізації")
# Кнопка завантаження файлу
uploaded_file = st.file_uploader("Оберіть .BIN файл", type=["BIN"])

if uploaded_file is not None:
    st.success(f"Файл завантажено: {uploaded_file.name}")
    with st.spinner("Читаємо дані..."):
        df = load_bin_file(uploaded_file)
    if df.empty:
        st.error("Не вдалось прочитати GPS дані")
    else: # Перша точка (0,0,0)
        lat0, lon0, alt0 = df['lat'].iloc[0], df['lon'].iloc[0], df['alt'].iloc[0]
        df['east'], df['north'], df['up'] = wgs84_to_enu( # Переводимо всі точки у метри
            df['lat'].values, df['lon'].values, df['alt'].values,
            lat0, lon0, alt0
        )
         # Будуємо 3D лінію траєкторії (колір залежить від швидкості)
        fig = go.Figure(data=go.Scatter3d(
            x=df['east'],  # вісь схід-захід
            y=df['north'], # вісь північ-південь
            z=df['up'], # висота
            mode='lines+markers',
            marker=dict(size=3, color=df['spd'], colorscale='Inferno',
                        colorbar=dict(title="Швидкість м/с")),
            line=dict(color=df['spd'], colorscale='Inferno', width=4)
        ))
        # Зелена точка - старт
        fig.add_trace(go.Scatter3d(
            x=[df['east'].iloc[0]], y=[df['north'].iloc[0]], z=[df['up'].iloc[0]],
            mode='markers', marker=dict(size=10, color='green'), name='Старт'
        ))
        # Червона точка - фініш (посадка)
        fig.add_trace(go.Scatter3d(
            x=[df['east'].iloc[-1]], y=[df['north'].iloc[-1]], z=[df['up'].iloc[-1]],
            mode='markers', marker=dict(size=10, color='red'), name='Фініш'
        ))
        # Налаштування осей графіку
        fig.update_layout(
            scene=dict(
                xaxis_title='Схід (м)',
                yaxis_title='Північ (м)',
                zaxis_title='Висота (м)',
                aspectmode='data' # пропорції без спотворень
            ),
            height=500 # висота графіку в пікселях 
        )
        # Показуємо графік на сторінці
        st.plotly_chart(fig, use_container_width=True)

