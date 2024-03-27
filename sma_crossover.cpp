#include <iostream>
#include <vector>
#include <numeric>
#include <tuple>

constexpr std::size_t SHORT_TERM_MA_LENGTH = 3;
constexpr std::size_t LONG_TERM_MA_LENGTH = 5;
constexpr std::size_t SIGNAL_PERIOD = 3;
constexpr std::size_t RSI_PERIOD = 14;
constexpr double OVERBOUGHT_THRESHOLD = 75.0;
constexpr double OVERSOLD_THRESHOLD = 25.0;

// Function to calculate the Simple Moving Average (SMA)
double calculateSMA(const std::vector<double>& data) {
	double sum = std::accumulate(data.begin(), data.end(), 0);
    return sum / data.size();
}
// Function to calculate the RSI and MACD
std::tuple<double, double> calculateRSIAndMACD(const std::vector<double>& data) {
    double sumGain = 0.0;
    double sumLoss = 0.0;
    double prevPrice = data.front();
    std::vector<double> shortTermEMA, longTermEMA, macd;
    const double shortTermSmoothingFactor = 2.0 / (SHORT_TERM_MA_LENGTH + 1);
    const double longTermSmoothingFactor = 2.0 / (LONG_TERM_MA_LENGTH + 1);
    const double signalSmoothingFactor = 2.0 / (SIGNAL_PERIOD + 1);

    // Calculate RSI and short-term EMA
    for (std::size_t i = 1; i < RSI_PERIOD; i++) {
        double diff = data[i] - prevPrice;
        if (diff > 0) {
            sumGain += diff;
        } else {
            sumLoss += std::abs(diff);
        }
        prevPrice = data[i];
        shortTermEMA.emplace_back(data[i]);
    }
    double avgGain = sumGain / RSI_PERIOD;
    double avgLoss = sumLoss / RSI_PERIOD;
    double rs = avgGain / avgLoss;
    double rsi = 100 - (100 / (1 + rs));

    // Calculate long-term EMA, MACD line, and signal line
    longTermEMA.emplace_back(data.front());
    for (const auto& price : data) {
        double ema = (price - longTermEMA.back()) * longTermSmoothingFactor + longTermEMA.back();
        longTermEMA.emplace_back(ema);

        if (shortTermEMA.size() == SHORT_TERM_MA_LENGTH) {
            shortTermEMA.erase(shortTermEMA.begin());
        }
        double ema2 = (price - shortTermEMA.back()) * shortTermSmoothingFactor + shortTermEMA.back();
        shortTermEMA.emplace_back(ema2);

        if (longTermEMA.size() > shortTermEMA.size()) {
            longTermEMA.erase(longTermEMA.begin());
        }
        if (shortTermEMA.size() == longTermEMA.size()) {
            macd.emplace_back(shortTermEMA.back() - longTermEMA.back());
        }
        if (macd.size() == SIGNAL_PERIOD) {
            macd.erase(macd.begin());
        }
        if (macd.size() == SIGNAL_PERIOD - 1) {
            double signal = std::accumulate(macd.begin(), macd.end(), 0.0) / SIGNAL_PERIOD;
            double currentMacd = macd.back();
            return std::make_tuple(rsi, currentMacd - signal);
        }
    }
    return std::make_tuple(rsi, 0.0);
}


// Function to generate trading signals based on SMA and RSI
int generateSignal(const std::vector<double>& stockPrices, const std::vector<double>& shortTermMA, const std::vector<double>& longTermMA, int RSI_PERIOD, double overboughtThreshold, double oversoldThreshold) {
    auto currentShortTermMA = shortTermMA.back();
    auto currentLongTermMA = longTermMA.back();
    auto previousShortTermMA = shortTermMA[shortTermMA.size() - 2];
    auto previousLongTermMA = longTermMA[longTermMA.size() - 2];

    std::vector<double> rsiPrices(stockPrices.end() - RSI_PERIOD, stockPrices.end());
    std::tuple<double, double> rsiAndMacd = calculateRSIAndMACD(rsiPrices);
    double rsi = std::get<0>(rsiAndMacd);
    double macd = std::get<1>(rsiAndMacd);

    if (currentShortTermMA > currentLongTermMA && previousShortTermMA <= previousLongTermMA && rsi <= oversoldThreshold) {
        return 1; // Buy signal
    } else if (currentShortTermMA < currentLongTermMA && previousShortTermMA >= previousLongTermMA && rsi >= overboughtThreshold) {
        return -1; // Sell signal
    } else {
        return 0; // No signal
    }
}

int main() {
    std::vector<double> stockPrices = { 100.0, 110.0, 120.0, 130.0, 140.0, 130.0, 120.0, 110.0, 100.0, 90.0 };

    // Calculate the short-term and long-term moving averages
    std::vector<double> shortTermMA;
    std::vector<double> longTermMA;
    std::size_t maxIndex = std::max(SHORT_TERM_MA_LENGTH, LONG_TERM_MA_LENGTH);
    for (std::size_t i = maxIndex; i < stockPrices.size(); i++) {
		std::vector<double> shortTermPrices(stockPrices.begin() + i - SHORT_TERM_MA_LENGTH, stockPrices.begin() + i);
		std::vector<double> longTermPrices(stockPrices.begin() + i - LONG_TERM_MA_LENGTH, stockPrices.begin() + i);

        shortTermMA.push_back(calculateSMA(shortTermPrices));
        longTermMA.push_back(calculateSMA(longTermPrices));
    }

    // Generate trading signals based on SMA crossover
    int signal = generateSignal(stockPrices, shortTermMA, longTermMA, RSI_PERIOD, OVERBOUGHT_THRESHOLD, OVERSOLD_THRESHOLD);
    if (signal == 1) {
        std::cout << "Buy signal generated!" << std::endl;
    } else if (signal == -1) {
        std::cout << "Sell signal generated!" << std::endl;
    } else {
        std::cout << "No signal generated." << std::endl;
    }
    return 0;
}
