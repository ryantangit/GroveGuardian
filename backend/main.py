from fastapi import FastAPI, HTTPException, Depends
from pydantic import BaseModel
from sqlalchemy import Column, Integer, Float, String, create_engine, DateTime
from sqlalchemy.orm import sessionmaker, declarative_base
from sqlalchemy.orm import sessionmaker, Session
from sqlalchemy import desc
from datetime import datetime, timezone

database_url = "sqlite:///./hello_test.db"
engine = create_engine(database_url, connect_args={"check_same_thread": False})
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
Base = declarative_base()

class HelloTest(Base):
    __tablename__ = "hello_data"

    id = Column(Integer, primary_key=True, index=True)
    message = Column(String)  
    timestamp = Column(DateTime, default=datetime.now(timezone.utc))

Base.metadata.create_all(bind=engine)

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

app = FastAPI()

from fastapi.middleware.cors import CORSMiddleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Allows all origins; use specific origins in production
    allow_credentials=True,
    allow_methods=["*"],  # Allows all methods
    allow_headers=["*"],  # Allows all headers
)


# @app.get("/ping")
# def ping():
#     return "pong"

# @app.post("/test")
# def post_test():
#     return

# Data class for test update
class TestUpdateRequest(BaseModel):
    message: str

    class Config:
        from_attributes = True


# Post endpoint for test update
@app.post("/testUpdate")
def test_update(request: TestUpdateRequest, db: Session = Depends(get_db)):
    new_record = HelloTest(message=request.message, timestamp=datetime.now(timezone.utc))
    # create new endpt
    # unwrap message to int here
    # query most recent entry
    # compare percemtages --> if difference is >=10, then only post, else dont update
    db.add(new_record)
    db.commit()
    db.refresh(new_record)  
    
    # only return time + percentage
    return {"id": new_record.id, "message": new_record.message, "timestamp": new_record.timestamp} 

# Frontend Calls this
# Get most recent entry from the database 
@app.get("/getUpdate")
def get_update(db: Session = Depends(get_db)):
    recent_entry = db.query(HelloTest).order_by(desc(HelloTest.timestamp)).first()

    if recent_entry is None:
        raise HTTPException(status_code=404, detail="No messages found")

    return {
        "id": recent_entry.id,
        "message": recent_entry.message,
        "timestamp": recent_entry.timestamp
    }

# Sensor calls this endpt
@app.post("/smartUpdate")
def smart_update(request: TestUpdateRequest, db: Session = Depends(get_db)):
    try:
        new_value = int(request.message)
    except ValueError:
        raise HTTPException(status_code=400, detail="Message must be a numeric string")

    # Get the most recent reading
    recent_entry = db.query(HelloTest).order_by(desc(HelloTest.timestamp)).first()

    if recent_entry:
        try:
            recent_value = int(recent_entry.message)
        except ValueError:
            recent_value = 0  # Fallback if DB entry was corrupted or bad format

        if abs(new_value - recent_value) < 2:
            return {"message": "No significant change"}
        
        if new_value < 0:
            new_value = 0

    # If difference is ≥ 2 or there's no previous entry, save it
    new_record = HelloTest(
        message=str(new_value),
        timestamp=datetime.now(timezone.utc)
    )
    db.add(new_record)
    db.commit()
    db.refresh(new_record)

    return {
        "message": new_value,
        "timestamp": new_record.timestamp
    }
