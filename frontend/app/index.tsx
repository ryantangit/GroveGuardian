import React, { useEffect, useState } from "react";
import { View, Text, FlatList, StyleSheet, Dimensions } from "react-native";
import { LinearGradient } from "expo-linear-gradient";
import { Ionicons } from "@expo/vector-icons";

const { width } = Dimensions.get("window");

export default function PlantDashboard() {
  const [plantReadings, setPlantReadings] = useState<any[]>([]);
  const [statusColor, setStatusColor] = useState(["#4CAF50", "#2E7D32"]);
  const [statusText, setStatusText] = useState("Healthy & Thriving");

  useEffect(() => {
    const fetchRecentReading = async () => {
      try {
        // for local use:
        const response = await fetch("http://10.0.0.207:8000/getUpdate");
        // for cloud use:
        // http://4.242.221.245:8000/getUpdate
        const data = await response.json();

        if (data) {
          const moisture = parseInt(data.message);
          const { status, gradient } = getStatusProps(moisture);

          setStatusText(status);
          setStatusColor(gradient);

          setPlantReadings((prevReadings) => {
            const isDuplicate =
              prevReadings.length > 0 &&
              prevReadings[0].timestamp === data.timestamp;

            if (!isDuplicate) {
              return [data, ...prevReadings];
            } else {
              return prevReadings;
            }
          });
        }
      } catch (error) {
        console.error("Error fetching data: ", error);
      }
    };

    fetchRecentReading();
    const interval = setInterval(fetchRecentReading, 1000);
    return () => clearInterval(interval);
  }, []);

  return (
    <View style={styles.container}>
      <LinearGradient colors={statusColor} style={styles.statusCard}>
        <Ionicons name="leaf" size={width * 0.12} color="white" />
        <Text style={styles.statusTitle}>{statusText}</Text>
        {plantReadings[0] && (
          <Text style={styles.statusDetail}>
            Moisture Level: {plantReadings[0].message}%
          </Text>
        )}
      </LinearGradient>

      <Text style={styles.readingsTitle}>Recent Moisture Readings</Text>
      <FlatList
        data={plantReadings}
        keyExtractor={(item, index) => index.toString()}
        renderItem={({ item }) => {
          const moisture = parseInt(item.message);
          const { label, color } = getStatusDisplay(moisture);

          return (
            <View style={styles.readingItem}>
              <View style={styles.readingLeft}>
                <Ionicons name="calendar" size={width * 0.06} color="#4CAF50" />
                <Text style={styles.readingDate}>{item.timestamp}</Text>
              </View>
              <View style={styles.readingRight}>
                <Text style={styles.readingMoisture}>{item.message}%</Text>
                <Text style={[styles.readingStatus, { color }]}>{label}</Text>
              </View>
            </View>
          );
        }}
      />
    </View>
  );
}

const getStatusProps = (moisture: number) => {
  if (moisture <= 30 || moisture > 60) {
    return {
      status: "Warning: Check Moisture",
      gradient: ["#FFD54F", "#FFA000"], // Yellow
    };
  }
  return {
    status: "Healthy & Thriving",
    gradient: ["#4CAF50", "#2E7D32"],
  };
};

const getStatusDisplay = (moisture: number) => {
  if (moisture <= 30) {
    return { label: "Status: Too Dry", color: "#FFA000" }; 
  } else if (moisture > 60) {
    return { label: "Status: Too Wet", color: "#FFA000" };
  } else {
    return { label: "Status: Good", color: "#4CAF50" }; 
  }
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#FAFAFA",
    paddingHorizontal: "5%",
    paddingTop: "20%",
  },
  statusCard: {
    width: "100%",
    borderRadius: 15,
    paddingVertical: "6%",
    alignItems: "center",
    justifyContent: "center",
    shadowColor: "#000",
    shadowOpacity: 0.2,
    shadowRadius: 10,
    elevation: 5,
  },
  statusTitle: {
    color: "white",
    fontSize: width * 0.06,
    fontWeight: "bold",
    marginTop: "2%",
  },
  statusDetail: {
    color: "white",
    fontSize: width * 0.045,
    opacity: 0.8,
  },
  readingsTitle: {
    fontSize: width * 0.05,
    fontWeight: "bold",
    marginVertical: "5%",
    color: "#333",
  },
  readingItem: {
    backgroundColor: "#FFFFFF",
    paddingVertical: "5%",
    paddingHorizontal: "6%",
    marginVertical: "1.5%",
    borderRadius: 12,
    shadowColor: "#000",
    shadowOpacity: 0.1,
    shadowRadius: 5,
    elevation: 3,
  },
  readingLeft: {
    flexDirection: "row",
    alignItems: "center",
  },
  readingDate: {
    fontSize: width * 0.04,
    marginLeft: "3%",
    color: "#333",
    fontWeight: "500",
  },
  readingRight: {
    flexDirection: "column",
    alignItems: "flex-start",
    marginTop: "5%",
  },
  readingMoisture: {
    fontSize: width * 0.045,
    fontWeight: "bold",
    color: "#000000",
    marginBottom: "2%",
  },
  readingStatus: {
    fontSize: width * 0.04,
    fontWeight: "bold",
  },
});
