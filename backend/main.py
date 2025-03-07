from fastapi import FastAPI, HTTPException, Depends
from pydantic import BaseModel
from sqlalchemy import Column, Integer, Float, String, create_engine, DateTime
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker, Session
from datetime import datetime, timezone

database_url = "sqlite:///./plant_moisture.db"
engine = create_engine(database_url, connect_args={"check_same_thread": False})
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
Base = declarative_base()

class PlantMoisture(Base):
    __tablename__ = "moisture_data"

    id = Column(Integer, primary_key=True, index=True)
    moisture_level = Column(Float)  
timestamp = Column(DateTime, default=datetime.now(timezone.utc))

Base.metadata.create_all(bind=engine)

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

app = FastAPI()

class MoistureCreate(BaseModel):
    moisture_level: float

    class Config:
        orm_mode = True

@app.get("/ping/")
def ping():
    return {"message": "pong"}

@app.post("/moisture/")
def create_moisture_data(moisture: MoistureCreate, db: Session = Depends(get_db)):
    db_moisture = PlantMoisture(moisture_level=moisture.moisture_level)
    db.add(db_moisture)
    db.commit()
    db.refresh(db_moisture)
    return db_moisture

@app.get("/moisture/")
def get_moisture_data(db: Session = Depends(get_db)):
    moisture_data = db.query(PlantMoisture).order_by(PlantMoisture.timestamp.desc()).limit(10).all()  # Last 10 readings
    if not moisture_data:
        raise HTTPException(status_code=404, detail="No moisture data found")
    return moisture_data
