# Підключаємо бібліотеки
import streamlit as st  # робить сайт у браузері
import json             # читає JSON файл від команди
import pandas as pd     # таблиці

# Створюємо сторінку
st.set_page_config(page_title="Метрики польоту БПЛА", layout="wide")
st.title("📊 Аналіз польоту БПЛА")
st.write("Дані отримані з бортового журналу польотного контролера Ardupilot")

# Кнопка завантаження JSON файлу через браузер
uploaded_json = st.file_uploader("Завантажте metrics.json", type=["json"])

if uploaded_json is not None:
    # Читаємо метрики з JSON файлу який згенерував C++
    metrics = json.load(uploaded_json)
    st.success("Дані успішно завантажені")

    # Показники польоту — картки з цифрами
    st.subheader("Показники польоту")
    col1, col2, col3, col4 = st.columns(4)
    col1.metric("Макс. швидкість",   f"{metrics['max_speed']:.1f} м/с")
    col2.metric("Дистанція",         f"{metrics['total_distance']:.0f} м")
    # Переводимо секунди у хвилини і секунди
    col3.metric("Тривалість",        f"{int(metrics['flight_duration'] // 60)}:{int(metrics['flight_duration'] % 60):02d}")
    col4.metric("Макс. прискорення", f"{metrics['max_acceleration']:.1f} м/с²")

    # Таблиця з усіма даними
    st.subheader("Детальні дані")
    df = pd.DataFrame([{
        "Показник": "Макс. швидкість",
        "Значення": f"{metrics['max_speed']:.1f}",
        "Одиниці": "м/с"
    }, {
        "Показник": "Загальна дистанція",
        "Значення": f"{metrics['total_distance']:.1f}",
        "Одиниці": "м"
    }, {
        "Показник": "Тривалість польоту",
        "Значення": f"{metrics['flight_duration']:.1f}",
        "Одиниці": "с"
    }, {
        "Показник": "Макс. прискорення",
        "Значення": f"{metrics['max_acceleration']:.1f}",
        "Одиниці": "м/с²"
    }])
    st.dataframe(df, use_container_width=True)

else:
    st.info("Завантажте metrics.json щоб побачити результати аналізу польоту")

# streamlit run metrics.py
