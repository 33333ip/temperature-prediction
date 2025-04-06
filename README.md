# Temperature Forecasting Using ML and LSTM

This project aims to predict temperature 10 minutes into the future using time-series data. Several models were tested and compared, including Linear Regression, Support Vector Regression (SVR), and a Long Short-Term Memory (LSTM) neural network.

## Dataset
- Minute-level temperature data
- Source: Sensor log file

## Models Used
- Linear Regression (using last 5 & 50 points)
- Support Vector Regression (SVR)
- LSTM Neural Network

## Project Structure
- `notebooks/`: Jupyter Notebook with model implementation and evaluation
- `plots/`: Visualization of training loss and predictions
- `data/`: Raw dataset (excluded in `.gitignore` if necessary)
- `requirements.txt`: Required packages

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
| **LSTM** | **0.51** | **0.60** |

> LSTM achieved the best performance thanks to its ability to learn sequential patterns.

## Author
Nitchamon Busayaphongchai

## How to Run
```bash
pip install -r requirements.txt
