# Temperature Forecasting Using ML and LSTM

This project aims to predict temperature 10 minutes into the future using time-series data. Several models were tested and compared, including Linear Regression, Support Vector Regression (SVR), and a Long Short-Term Memory (LSTM) neural network.

Temperature data is collected every minute using a **Cucumber Board (ESP32-S2 Dev Module)** equipped with temperature sensors. The data is transmitted via **HTTP (REST)** and automatically logged to **Google Sheets**. This setup simulates a real-time IoT environment and provides rich time-series data for training and evaluating machine learning models.

## Dataset

- Source: Temperature readings collected every 1 minute
- Hardware: **Cucumber Board / ESP32-S2 Dev Module**
- Protocol: **HTTP** to Google Sheets
- Fields: Timestamp, Temperature, Humidity

## Models Used

- Linear Regression (5 and 50-minute input)
- Support Vector Regression (SVR) (5 and 50-minute input)
- LSTM Neural Network (60-minute input)

## Evaluation Metrics

- MAE: Mean Absolute Error
- RMSE: Root Mean Squared Error

## Results Summary

| Model    | MAE    | RMSE   |
|----------|--------|--------|
| LR-5     | 0.99   | 1.65   |
| LR-50    | 0.88   | 1.43   |
| SVR-5    | 0.75   | 1.40   |
| SVR-50   | 0.62   | 1.14   |
| **LSTM** | **0.51**  | **0.60**  |

> LSTM achieved the best performance thanks to its ability to learn sequential patterns.

## How to Run

1. Install dependencies:
```bash
pip install -r requirements.txt
```

2. Run the Jupyter Notebook:
```bash
jupyter notebook notebooks/project3.ipynb
```

## Author

Nitchamon Busayaphongchai
