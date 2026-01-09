
import express, { Router } from 'express';
import { AlarmTypesController } from '../../controllers/AlarmTypesController';
import { asyncHandlerMiddleware } from '../../middlewares/asyncHandlerMiddleware';


const router = express.Router();
const alarmTypesController = new AlarmTypesController();
router.get("/ping", asyncHandlerMiddleware(alarmTypesController.ping.bind(alarmTypesController)));

router.post("/echo", asyncHandlerMiddleware(alarmTypesController.echo.bind(alarmTypesController)));

router.get("/status", asyncHandlerMiddleware(alarmTypesController.status.bind(alarmTypesController)));

router.get("/", asyncHandlerMiddleware(alarmTypesController.getAlarmTypes.bind(alarmTypesController)));

router.post("/", asyncHandlerMiddleware(alarmTypesController.createAlarmTypes.bind(alarmTypesController)));

router.put("/:id", asyncHandlerMiddleware(alarmTypesController.updateAlarmTypes.bind(alarmTypesController)));
export default router;