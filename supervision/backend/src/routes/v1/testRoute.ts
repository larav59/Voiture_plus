
import express from 'express';
import { TestController } from '../../controllers/TestController';


const router = express.Router();
const testController = new TestController();

router.get("/ping", testController.ping);

router.post("/echo", testController.echo);

router.get("/status", testController.status);



export default router;