fft_data = read.csv(
  "C:/Users/hkrug/OneDrive - James Cook University/2024 - SP2/CC3501- Computer Interfacing and Control/Labs/CC3501-RP2040-Labs/tests/logs/fft/fft.log", 
  header=FALSE
)
x = 1:length(fft_data$V1)
plot(x,fft_data$V1)
