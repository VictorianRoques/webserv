import requests

x = requests.get('http://localhost:8080/index.html')
print(x.status_code)
print(x.headers)
print(x.text)