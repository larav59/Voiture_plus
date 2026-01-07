
import express, { Router } from 'express';
import { TravelController } from '../../controllers/TravelController';
import { asyncHandlerMiddleware } from '../../middlewares/asyncHandlerMiddleware';


const router = express.Router();
const travelController = new TravelController();
router.get("/ping", asyncHandlerMiddleware(travelController.ping.bind(travelController)));

router.post("/echo", asyncHandlerMiddleware(travelController.echo.bind(travelController)));

router.get("/status", asyncHandlerMiddleware(travelController.status.bind(travelController)));

router.get("/", asyncHandlerMiddleware(travelController.getTravels.bind(travelController)));

router.post("/", asyncHandlerMiddleware(travelController.createTravel.bind(travelController)));

router.put("/:id", asyncHandlerMiddleware(travelController.updateTravel.bind(travelController)));
export default router;