import streamlit as st
import plotly.graph_objects as go
import numpy as np
import pandas as pd
from pymavlink import mavutil # читання .BIN файлів Ardupilot

def load_bin_file(uploaded_file):
    import tempfile, os
    with tempfile.NamedTemporaryFile(delete=False, suffix='.BIN') as tmp:
        tmp.write(uploaded_file.read())
        tmp_path = tmp.name
    log = mavutil.mavlink_connection(tmp_path)
    gps_data = []
    while True:
        msg = log.recv_match(type='GPS', blocking=False)
        if msg is None:
            break
        if msg.Status >= 3:
            gps_data.append({
                'lat': msg.Lat, # широта
                'lon': msg.Lng, # довгота
                'alt': msg.Alt, # висота в метрах
                'spd': msg.Spd, # швидкість в м/с
                'time_us': msg.TimeUS # час в мікросекундах
            })
    os.unlink(tmp_path)
    return pd.DataFrame(gps_data)

def haversine(lat1, lon1, lat2, lon2):
    R = 6378137.0 # радіус Землі в метрах
    dlat = np.radians(lat2 - lat1)
    dlon = np.radians(lon2 - lon1)
    a = np.sin(dlat/2)**2 + np.cos(np.radians(lat1)) * np.cos(np.radians(lat2)) * np.sin(dlon/2)**2
    return R * 2 * np.arcsin(np.sqrt(a))

def total_distance(df):
    dist = 0
    for i in range(1, len(df)):
        dist += haversine(
            df['lat'].iloc[i-1], df['lon'].iloc[i-1],
            df['lat'].iloc[i],   df['lon'].iloc[i]
        )
    return dist

def wgs84_to_enu(lat, lon, alt, lat0, lon0, alt0):
    R = 6378137.0 # радіус Землі в метрах
    east  = R * np.radians(lon - lon0) * np.cos(np.radians(lat0))
    north = R * np.radians(lat - lat0)
    up    = alt - alt0
    return east, north, up

st.set_page_config(page_title="Аналіз польоту БПЛА", layout="wide")
st.title("Аналіз польоту БПЛА")
st.write("Завантажте .BIN файл для початку аналізу")

uploaded_file = st.file_uploader("Оберіть .BIN файл", type=["BIN"])

if uploaded_file is not None:
    st.success(f"Файл завантажено: {uploaded_file.name}")

    with st.spinner("Читаємо дані з файлу..."):
        df = load_bin_file(uploaded_file)

    if df.empty:
        st.error("Не вдалось прочитати GPS дані з файлу")
    else:
        lat0, lon0, alt0 = df['lat'].iloc[0], df['lon'].iloc[0], df['alt'].iloc[0]
        df['east'], df['north'], df['up'] = wgs84_to_enu(
            df['lat'].values, df['lon'].values, df['alt'].values,
            lat0, lon0, alt0
        )

        dist_m = total_distance(df)
        duration_s = (df['time_us'].iloc[-1] - df['time_us'].iloc[0]) / 1_000_000
        minutes = int(duration_s // 60)
        seconds = int(duration_s % 60)

        st.subheader("Показники польоту")
        col1, col2, col3, col4, col5, col6 = st.columns(6)
        col1.metric("Макс. швидкість", f"{df['spd'].max():.1f} м/с")
        col2.metric("Макс. висота",    f"{df['alt'].max():.1f} м")
        col3.metric("Дистанція",       f"{dist_m:.0f} м")
        col4.metric("Тривалість",      f"{minutes}:{seconds:02d}")
        col5.metric("Перепад висоти",  f"{df['alt'].max() - df['alt'].min():.1f} м")
        col6, = st.columns(1)
        max_acceleration = 0.0   ## заглушка
        col6.metric("Макс. прискорення", f"{max_acceleration:.1f} м/с²")

        st.subheader("3D Траєкторія польоту")
        fig = go.Figure(data=go.Scatter3d(
            x=df['east'],
            y=df['north'],
            z=df['up'],
            mode='lines+markers',
            marker=dict(size=3, color=df['spd'], colorscale='Inferno',
                        colorbar=dict(title="Швидкість м/с")),
            line=dict(color=df['spd'], colorscale='Inferno', width=4)
        ))
        fig.add_trace(go.Scatter3d( # місце старту
            x=[df['east'].iloc[0]], y=[df['north'].iloc[0]], z=[df['up'].iloc[0]],
            mode='markers', marker=dict(size=10, color='green'), name='Старт'
        ))
        fig.add_trace(go.Scatter3d( # місце посадки
            x=[df['east'].iloc[-1]], y=[df['north'].iloc[-1]], z=[df['up'].iloc[-1]],
            mode='markers', marker=dict(size=10, color='red'), name='Фініш'
        ))
        fig.update_layout( # осі графіку
            scene=dict(
                xaxis_title='Схід (м)',
                yaxis_title='Північ (м)',
                zaxis_title='Висота (м)',
                aspectmode='data'
            ),
            height=500
        )
        st.plotly_chart(fig, use_container_width=True)
      
   
