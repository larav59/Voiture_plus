
import express, { Router } from 'express';
import { VehicleController } from '../../controllers/VehicleController';
import { asyncHandlerMiddleware } from '../../middlewares/asyncHandlerMiddleware';


const router = express.Router();
const vehicleController = new VehicleController();
router.get("/ping", asyncHandlerMiddleware(vehicleController.ping.bind(vehicleController)));

router.post("/echo", asyncHandlerMiddleware(vehicleController.echo.bind(vehicleController)));

router.get("/status", vehicleController.status);

router.get("/", asyncHandlerMiddleware(vehicleController.getVehicles.bind(vehicleController)));

router.post("/", asyncHandlerMiddleware(vehicleController.createVehicle.bind(vehicleController)));

router.put("/:id", asyncHandlerMiddleware(vehicleController.updateVehicle.bind(vehicleController)));




export default router;