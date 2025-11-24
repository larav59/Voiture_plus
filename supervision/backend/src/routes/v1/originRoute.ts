
import express, { Router } from 'express';
import { OriginController } from '../../controllers/OriginController';
import { asyncHandlerMiddleware } from '../../middlewares/asyncHandlerMiddleware';


const router = express.Router();
const originController = new OriginController();
router.get("/ping", asyncHandlerMiddleware(originController.ping.bind(originController)));

router.post("/echo", asyncHandlerMiddleware(originController.echo.bind(originController)));

router.get("/status", asyncHandlerMiddleware(originController.status.bind(originController)));

router.get("/", asyncHandlerMiddleware(originController.getOrigins.bind(originController)));

router.post("/", asyncHandlerMiddleware(originController.createOrigin.bind(originController)));

router.put("/:id", asyncHandlerMiddleware(originController.updateOrigin.bind(originController)));
export default router;