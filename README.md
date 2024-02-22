To run
```sh
make
gcc test.c -o test
sudo insmod ioctldriver.ko
sudo ./test
```

To remove device
```sh
sudo rmmod ioctldriver
```
