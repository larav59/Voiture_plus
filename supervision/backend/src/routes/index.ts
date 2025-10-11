import express from 'express';
import v1Routes from './v1/index';

const router = express.Router();

// All API versions are imported here and used as middleware
router.use('/v1', v1Routes);


export default router;