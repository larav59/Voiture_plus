
import express, { Router } from 'express';
import { AlarmController } from '../../controllers/AlarmController';
import { asyncHandlerMiddleware } from '../../middlewares/asyncHandlerMiddleware';


const router = express.Router();
const alarmController = new AlarmController();
router.get("/ping", asyncHandlerMiddleware(alarmController.ping.bind(alarmController)));

router.post("/echo", asyncHandlerMiddleware(alarmController.echo.bind(alarmController)));

router.get("/status", alarmController.status);

router.get("/", asyncHandlerMiddleware(alarmController.getAlarms.bind(alarmController)));

router.post("/", asyncHandlerMiddleware(alarmController.createAlarm.bind(alarmController)));

router.put("/:id", asyncHandlerMiddleware(alarmController.updateAlarm.bind(alarmController)));




export default router;