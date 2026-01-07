
import express, { Router } from 'express';
import { StateController } from '../../controllers/StateController';
import { asyncHandlerMiddleware } from '../../middlewares/asyncHandlerMiddleware';


const router = express.Router();
const stateController = new StateController();
router.get("/ping", asyncHandlerMiddleware(stateController.ping.bind(stateController)));

router.post("/echo", asyncHandlerMiddleware(stateController.echo.bind(stateController)));

router.get("/status", asyncHandlerMiddleware(stateController.status.bind(stateController)));

router.get("/", asyncHandlerMiddleware(stateController.getStates.bind(stateController)));


export default router;