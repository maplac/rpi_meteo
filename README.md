# rpi_meteo

Arduino UNO project in test folder is based on https://github.com/bjarne-hansen/py-nrf24/blob/master/arduino/simple-sender/simple-sender.ino

Testing Python script rf24_receiver_01.py is based on https://github.com/bjarne-hansen/py-nrf24/blob/master/test/int-receiver.py


Installation

Add cron jobs.
$ crontab -e
* * * * * python3 /home/pi/rpi_meteo/python/lcd_driver.py >/dev/null 2>&1
* * * * * python3 /home/pi/rpi_meteo/python/bme280_reader.py >/dev/null 2>&1

Make sure the pigpiod service is enabled and running.
$ systemctl status pigpiod

Install service.
sudo cp /home/pi/rpi_meteo/python/rf24_receiver.service /lib/systemd/system/rf24_receiver.service
sudo systemctl start rf24_receiver
sudo systemctl enable rf24_receiver

