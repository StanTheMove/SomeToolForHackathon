# Підключаємо бібліотеки
import streamlit as st             # робить сайт у браузері
import plotly.graph_objects as go  # малює 3D графік
import numpy as np                 # математика
import pandas as pd                # таблиці
import json                        # читає JSON файл від команди

# Переводить GPS координати з градусів у метри відносно точки старту
# WGS-84 = глобальна система GPS, ENU = локальна система в метрах
def wgs84_to_enu(lat, lon, alt, lat0, lon0, alt0):
    R = 6378137.0  # реальний радіус Землі в метрах
    east  = R * np.radians(lon - lon0) * np.cos(np.radians(lat0))  # метри на схід
    north = R * np.radians(lat - lat0)  # метри на північ
    up    = alt - alt0                  # метри вгору від старту
    return east, north, up

# Створюємо сторінку
st.set_page_config(page_title="3D Траєкторія БПЛА", layout="wide")
st.title("🗺️ 3D Траєкторія польоту БПЛА")
st.write("Дані отримані з бортового журналу польотного контролера Ardupilot")

# Кнопка завантаження JSON файлу через браузер
uploaded_json = st.file_uploader("Завантажте metrics.json", type=["json"])

if uploaded_json is not None:
    data = json.load(uploaded_json)
    st.success("Дані успішно завантажені")

    # Перевіряємо чи є GPS точки у JSON
    if "gps_points" not in data:
        st.error("У файлі немає GPS точок. Попросіть команду додати gps_points у JSON")
        st.stop()

    # Перетворюємо GPS точки у таблицю
    df = pd.DataFrame(data["gps_points"])

    # Перша точка = точка старту = (0, 0, 0)
    lat0, lon0, alt0 = df['lat'].iloc[0], df['lon'].iloc[0], df['alt'].iloc[0]

    # Переводимо всі GPS точки з градусів у метри відносно старту
    df['east'], df['north'], df['up'] = wgs84_to_enu(
        df['lat'].values, df['lon'].values, df['alt'].values,
        lat0, lon0, alt0
    )

    # Будуємо 3D траєкторію — колір залежить від висоти
    st.subheader("3D Траєкторія польоту")
    fig = go.Figure(data=go.Scatter3d(
        x=df['east'],   # вісь схід-захід в метрах
        y=df['north'],  # вісь північ-південь в метрах
        z=df['up'],     # висота в метрах
        mode='lines+markers',
        marker=dict(
            size=3,
            color=df['up'],          # колір залежить від висоти
            colorscale='Inferno',    # палітра кольорів — я вигадав
            colorbar=dict(title="Висота м")
        ),
        line=dict(color='darkblue', width=4)
    ))

    # Зелена точка — місце старту
    fig.add_trace(go.Scatter3d(
        x=[df['east'].iloc[0]], y=[df['north'].iloc[0]], z=[df['up'].iloc[0]],
        mode='markers', marker=dict(size=10, color='green'), name='Старт'
    ))

    # Червона точка — місце посадки
    fig.add_trace(go.Scatter3d(
        x=[df['east'].iloc[-1]], y=[df['north'].iloc[-1]], z=[df['up'].iloc[-1]],
        mode='markers', marker=dict(size=10, color='red'), name='Фініш'
    ))

    # Підписи осей і розмір графіку
    fig.update_layout(
        scene=dict(
            xaxis_title='Схід (м)',
            yaxis_title='Північ (м)',
            zaxis_title='Висота (м)',
            aspectmode='data'  # реальні пропорції без спотворень
        ),
        height=500  
    )

    # Показуємо графік на всю ширину сторінки
    st.plotly_chart(fig, use_container_width=True)

else:
    # Підказка якщо файл ще не завантажено
    st.info("Завантажте metrics.json щоб побачити 3D траєкторію")

# streamlit run dashboard.py