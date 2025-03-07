from fastapi import FastAPI 

app = FastAPI()

@app.get("/ping")
def ping():
    return "pong"

@app.post("/test")
def post_test():
    return
