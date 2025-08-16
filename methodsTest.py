import requests

BASE_URL = "http://localhost:8080"  # Change to your server's URL

def test_get():
    print("\n=== GET Request ===")
    try:
        r = requests.get(f"{BASE_URL}/uploads/postFile.html")
        print("Status:", r.status_code)
        print("Response:", r.text)
    except Exception as e:
        print("GET error:", e)

def test_post():
    print("\n=== POST Request ===")
    try:
        data = {"name": "Oussama", "message": "Hello from POST"}
        r = requests.post(f"{BASE_URL}/data", json=data)
        print("Status:", r.status_code)
        print("Response:", r.text)
    except Exception as e:
        print("POST error:", e)

def test_delete():
    print("\n=== DELETE Request ===")
    try:
        r = requests.delete(f"{BASE_URL}/1755109938.ser")
        print("Status:", r.status_code)
        print("Response:", r.text)
    except Exception as e:
        print("DELETE error:", e)

if __name__ == "__main__":
    test_get()
    test_post()
    test_delete()
