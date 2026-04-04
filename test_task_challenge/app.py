import json
import pymap3d as pm
import numpy as np
import plotly.graph_objects as go

#імпорт даних gps з json-файлу
with open("gps.json") as f:
    data = json.load(f)

#формування масивів координат(широта, довгота, висота у wgs-84) та часових міток
lats = np.array([p["lat"] for p in data])
lons = np.array([p["lon"] for p in data])
h = np.array([p["alt"] for p in data])
times = np.array([p["time"] for p in data])

lat0, lon0, h0 = lats[0], lons[0], h[0]  #задання точки відліку
e, n, u = pm.geodetic2enu(lats, lons, h, lat0, lon0, h0) #переведення з wgs-84 в enu за допомоги бібліотечних функцій


#обчислення відстані за формулою √(de^2 + dn^2 + du^2)
distances = np.sqrt(np.diff(e)**2 + np.diff(n)**2 + np.diff(u)**2)


dt = np.diff(times)
dt[dt == 0] = 1e-6

speed = np.zeros_like(e)
#обчислення швидкості та запис даних у масив
speed[1:] = distances / dt

time_stamps = times

#cтворення 3D-візуалізації
fig = go.Figure(data=[go.Scatter3d(
    x=e, y=n, z=u,
    mode = 'markers+lines',
    marker = dict(
        size = 4,
        color = speed, #параметр, у відношенні до якого відб. зміна кольору (тут: час)
        colorscale = 'Viridis',
        colorbar = dict(title = "Швидкість, м/с")
    ),
    text = time_stamps,
    hovertemplate = "Час: %{text}с<br>Швидкість: %{marker.color:.2f}м/с",
    line = dict(color = 'darkblue', width = 2)
)])

fig.update_layout(
    scene = dict(
        xaxis_title = 'East (м)',
        yaxis_title = 'North (м)',
        zaxis_title = 'Altitude (м)',
        aspectmode = 'data'
    ),
    title = "Інтерактивна 3D-траєкторія",
    template = "plotly_dark"
)
fig.show()
