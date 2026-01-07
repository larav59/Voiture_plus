
import express, { Router } from 'express';
import { VehicleController } from '../../controllers/VehicleController';
import { asyncHandlerMiddleware } from '../../middlewares/asyncHandlerMiddleware';
import { StateController } from '../../controllers/StateController';


const router = express.Router();
const vehicleController = new VehicleController();
const stateController = new StateController();
router.get("/ping", asyncHandlerMiddleware(vehicleController.ping.bind(vehicleController)));

router.post("/echo", asyncHandlerMiddleware(vehicleController.echo.bind(vehicleController)));

router.get("/status", vehicleController.status);

router.get("/", asyncHandlerMiddleware(vehicleController.getVehicles.bind(vehicleController)));

router.post("/", asyncHandlerMiddleware(vehicleController.createVehicle.bind(vehicleController)));

router.put("/:vehicleId", asyncHandlerMiddleware(vehicleController.updateVehicle.bind(vehicleController)));

router.post("/:vehicleId/state", asyncHandlerMiddleware(stateController.createState.bind(stateController)));

router.put("/:vehicleId/states/:stateId", asyncHandlerMiddleware(stateController.updateState.bind(stateController)));
export default router;