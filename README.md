# battery-charger
Kod ładowarki/rozładowarki akumulatorów dla ESP32, LCD ST7567, INA219, encoder, SD card. Zbudowałem/zaprogramowalem to bardziej dla sportu niż rzeczywistej chęci wykorzystania.
Do ładowarki potrzebny jest dowolny zasilacz 19V 4A

Funkcjonalność:
1. Ładowanie i rozładowanie akumulatorów kwasowo-ołowiowych oraz Li-po 2s i 3s.
2. Łączność WiFi, odczyt czasu, serwer OTA,
3. Pomiar napięcia, prądu, czasu ładowania, czasu trwania poszczególnych trybów ładowania
4. Zliczanie Ah, Wh
5. Zapis logów na SD w trybie automatycznym lub ręcznym, zapis podsumowania ładowania/rozładowania
6. Możliwość edytowania napięć granicznych dla poszczególnych typów akumulatorów i trybów (u_bulk, u_float)
7. Możliwość ustawienia rezystancji bocznika INA219
8. Możliwość wykorzystania ładowarki jako zasilacza dzięki regulacji PWM
9. Ustawianie w menu parametrów PID
10. 
