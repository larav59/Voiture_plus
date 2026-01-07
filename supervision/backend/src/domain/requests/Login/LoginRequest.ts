import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class LoginRequest extends BaseRequest<LoginRequest> {
    username : string ;
    password : string ;

    constructor(fields?: Partial<LoginRequest>) {
        super();
        this.username = fields?.username ?? "";
        this.password = fields?.password ?? "";
    }

    // Méthode pour la validation
    public validate(): Validator<LoginRequest> {
        const validator = new Validator<LoginRequest>(this);
        validator.field("username").isRequired().minLength(1).maxLength(100);
        validator.field("password").isRequired().minLength(1).maxLength(100);
        return validator;
    }

    static fromRequest(req: any): LoginRequest {
        return new LoginRequest({
            username: req.body.username,
            password: req.body.password,
        });
    }
}