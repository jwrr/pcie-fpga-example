How to Create Signing Key
=========================

* This is based on: https://unix.stackexchange.com/questions/751517/insmod-causes-key-rejected-by-service

* First, edit ```x509.genkey``` change JWRR to your name/organization.

* Next, run ```openssl``` to generate the signing key.

```bash
openssl req -x509 -new -nodes -utf8 -sha256 -days 36500 \
    -batch -config x509.genkey -outform DER \
    -out signing_key.x509 \
    -keyout signing_key.priv
```
* Now enroll the signing key. ```mokutil``` will ask for a one-time
  password. I use "12345678".  When you reboot a pop-up to Enroll
  MOK (Machine Owner's Key). and then it will ask for specific
  characters in the password. 

```
sudo mokutil --import signing_key.x509
sudo reboot
```

* Here are two commands to verify the signing key is installed.

```
sudo keyctl list %:.platform
sudo mokutil -l
```


