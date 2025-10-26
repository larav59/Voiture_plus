import { Column, Entity, OneToMany, PrimaryGeneratedColumn } from "typeorm";
import { Alarms } from "./Alarms";

@Entity("ALARMS_TYPES", { schema: "supervision" })
export class AlarmsTypes {
  @PrimaryGeneratedColumn({ type: "int", name: "id" })
  public id!: number;

  @Column("varchar", { name: "label", nullable: true, length: 255 })
  public label!: string | null;

  @Column("int", { name: "criticity", nullable: true })
  public criticity!: number | null;

  @OneToMany(() => Alarms, (alarms) => alarms.alarmType)
  public alarms!: Alarms[];

  public constructor(init?: Partial<AlarmsTypes>) {
    Object.assign(this, init);
  }
}
