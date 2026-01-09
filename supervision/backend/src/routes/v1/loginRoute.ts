
import express, { Router } from 'express';
import { LoginController } from '../../controllers/LoginController';
import { asyncHandlerMiddleware } from '../../middlewares/asyncHandlerMiddleware';


const router = express.Router();
const loginController = new LoginController();
router.get("/ping", asyncHandlerMiddleware(loginController.ping.bind(loginController)));

router.post("/echo", asyncHandlerMiddleware(loginController.echo.bind(loginController)));

router.get("/status", asyncHandlerMiddleware(loginController.status.bind(loginController)));

router.post("/", asyncHandlerMiddleware(loginController.login.bind(loginController)));


export default router;