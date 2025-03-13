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
   
    db.add(new_record)
    db.commit()
    db.refresh(new_record)  
    
    return {"id": new_record.id, "message": new_record.message, "timestamp": new_record.timestamp} 

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

