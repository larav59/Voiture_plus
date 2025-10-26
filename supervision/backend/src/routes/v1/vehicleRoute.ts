
import express from 'express';
import { VehicleController } from '../../controllers/VehicleController';


const router = express.Router();
const vehiculeController = new VehicleController();

router.get("/ping", vehiculeController.ping);

router.post("/echo", vehiculeController.echo);

router.get("/status", vehiculeController.status);

router.get("/", vehiculeController.getVehicules);



export default router;