from ultralytics import YOLO
YOLO("best/supercool_best.pt").export(format="imx")
