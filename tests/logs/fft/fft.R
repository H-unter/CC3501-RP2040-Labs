fft_data = read.csv(
  "C:/Users/hkrug/OneDrive - James Cook University/2024 - SP2/CC3501- Computer Interfacing and Control/Labs/CC3501-RP2040-Labs/tests/logs/fft/fft.log", 
  header=FALSE
)
t =  seq(1, length(fft_data$V1), by=1)
f = seq(1, length(fft_data$V2)/2 + 1, by=1)
plot(t,fft_data$V1)



real_indexes = seq(0, length(fft_data$V2), by=2)
complex_indexes = seq(1, length(fft_data$V2), by=2)
spectral_density = fft_data$V2[real_indexes]^2 + fft_data$V2[complex_indexes]^2
plot(f, spectral_density[1:length(f)], type="l", xlab="Frequency", ylab="Spectral Density")
