set terminal png
set output "Config1.png"
set title "Congestion Window Calculation for Configuration1"
set xlabel "Time (in Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "Third1.cwnd" using 1:2 with linespoints title "Connection1 Old Congestion","Third1.cwnd" using 1:3 with linespoints title "Connection1 New Congestion","Third2.cwnd" using 1:2 with linespoints title "Connection2 Old Congestion","Third2.cwnd" using 1:3 with linespoints title "Connection2 New Congestion","Third3.cwnd" using 1:2 with linespoints title "Connection3 Old Congestion","Third3.cwnd" using 1:3 with linespoints title "Connection3 New Congestion"